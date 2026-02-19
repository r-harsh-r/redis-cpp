#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <iomanip>
#include <random>
#include <string>
#include <algorithm>

using namespace std;

// Configuration
const string SERVER_HOST = "127.0.0.1";
const string SERVER_PORT = "6379";
const int NUM_OPERATIONS = 10000;
const int CONCURRENT_CLIENTS = 50;
const int VALUE_SIZE = 256;

// Statistics
struct BenchmarkStats {
    long long total_operations = 0;
    long long total_time_us = 0;
    double min_latency_us = 1e18;
    double max_latency_us = 0;
    vector<long long> latencies;
    mutex stats_lock;
};

BenchmarkStats set_stats, get_stats;

string generateRandomString(int length) {
    static thread_local mt19937 gen(random_device{}());
    static uniform_int_distribution<> dis(97, 122); 
    string result(length, '0');
    for (int i = 0; i < length; ++i) result[i] = static_cast<char>(dis(gen));
    return result;
}

int connectToServer() {
    struct addrinfo hints, *res, *ptr;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(SERVER_HOST.c_str(), SERVER_PORT.c_str(), &hints, &res) != 0) return -1;

    int serverFD = -1;
    for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
        serverFD = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (serverFD == -1) continue;
        if (connect(serverFD, ptr->ai_addr, ptr->ai_addrlen) != -1) break;
        close(serverFD);
    }
    freeaddrinfo(res);
    if (ptr == NULL) return -1;

    int flag = 1;
    setsockopt(serverFD, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

    return serverFD;
}

string customSerialize(const string& str) {
    int length = (int)str.length();
    return "$" + to_string(length) + "\r\n" + str + "\r\n";
}

bool sendAndReceiveCustom(int fd, const string& serialized_cmd) {
    // Send the command
    if (send(fd, serialized_cmd.c_str(), serialized_cmd.length(), 0) < 0) {
        return false;
    }

    char buffer[4096];
    string response = "";
    int expected_length = -1;
    bool header_parsed = false;

    // Loop until we have exactly 1 full message
    while (true) {
        int bytesRead = recv(fd, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) return false; // Connection closed or error
        // cout<<"In the loop, byte read : "<<bytesRead<<endl;
        response.append(buffer, bytesRead);

        // Try to parse the header if we haven't yet
        if (!header_parsed) {
            size_t pos = response.find("\r\n");
            if (pos != string::npos) {
                // We found the end of the header
                if (response[0] != '$') return false; // Protocol mismatch
                
                try {
                    // Extract length ("$2\r\n" -> "2")
                    string len_str = response.substr(1, pos - 1);
                    int data_len = stoi(len_str);
                    
                    // Total size = Header + \r\n + Payload + \r\n
                    expected_length = pos + 2 + data_len + 2; 
                    header_parsed = true;
                } catch (...) {
                    return false; 
                }
            }
        }

        // PARSED THE HEADER AND RECEIVED THE OUTPUT
        if (header_parsed && (int)response.length() >= expected_length) {
            // cout<<"Out of loop"<<endl;
            return true;
        }
    }
}

// Benchmark SET operations
void benchmarkSet(int client_id, int operations_per_client) {
    int fd = connectToServer();
    if (fd < 0) return;

    vector<long long> local_latencies;
    local_latencies.reserve(operations_per_client);
    
    for (int i = 0; i < operations_per_client; ++i) {
        string key = "key_" + to_string(client_id) + "_" + to_string(i);
        string value = generateRandomString(VALUE_SIZE);
        
        string cmd = customSerialize("SET " + key + " " + value);
        
        auto start = chrono::high_resolution_clock::now();
        bool success = sendAndReceiveCustom(fd, cmd);
        auto end = chrono::high_resolution_clock::now();
        
        if (success) {
            long long latency_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
            local_latencies.push_back(latency_us);
        } else {
            close(fd);
            fd = connectToServer(); // Reconnect on failure
            if (fd < 0) break;
        }
    }

    // thread safe stat aggregation
    lock_guard<mutex> lock(set_stats.stats_lock);
    set_stats.total_operations += local_latencies.size();
    for (long long lat : local_latencies) {
        set_stats.total_time_us += lat;
        set_stats.latencies.push_back(lat);
        set_stats.min_latency_us = min(set_stats.min_latency_us, (double)lat);
        set_stats.max_latency_us = max(set_stats.max_latency_us, (double)lat);
    }
    close(fd);
}

// Benchmark GET operations
void benchmarkGet(int client_id, int operations_per_client) {
    int fd = connectToServer();
    if (fd < 0) return;

    vector<long long> local_latencies;
    local_latencies.reserve(operations_per_client);
    
    for (int i = 0; i < operations_per_client; ++i) {
        string key = "key_" + to_string(client_id) + "_" + to_string(i);
        
        string cmd = customSerialize("GET " + key);
        
        auto start = chrono::high_resolution_clock::now();
        bool success = sendAndReceiveCustom(fd, cmd);
        auto end = chrono::high_resolution_clock::now();
        
        if (success) {
            long long latency_us = chrono::duration_cast<chrono::microseconds>(end - start).count();
            local_latencies.push_back(latency_us);
        } else {
            close(fd);
            fd = connectToServer(); // Reconnect on failure
            if (fd < 0) break;
        }
    }

    lock_guard<mutex> lock(get_stats.stats_lock);
    get_stats.total_operations += local_latencies.size();
    for (long long lat : local_latencies) {
        get_stats.total_time_us += lat;
        get_stats.latencies.push_back(lat);
        get_stats.min_latency_us = min(get_stats.min_latency_us, (double)lat);
        get_stats.max_latency_us = max(get_stats.max_latency_us, (double)lat);
    }
    close(fd);
}

double calculatePercentile(vector<long long>& data, double percentile) {
    if (data.empty()) return 0.0;
    int index = (int)((percentile / 100.0) * data.size());
    index = min(index, (int)data.size() - 1);
    return (double)data[index];
}

void printStats(const string& operation_name, BenchmarkStats& stats) {
    if (stats.total_operations == 0) return;

    double avg_latency_us = (double)stats.total_time_us / stats.total_operations;
    double throughput = (1000000.0 / avg_latency_us) * CONCURRENT_CLIENTS; 
    
    sort(stats.latencies.begin(), stats.latencies.end());
    
    double p50 = calculatePercentile(stats.latencies, 50);
    double p95 = calculatePercentile(stats.latencies, 95);
    double p99 = calculatePercentile(stats.latencies, 99);

    cout << "\n=== " << operation_name << " ===" << endl;
    cout << "Total Operations: " << stats.total_operations << endl;
    cout << "Throughput:       " << fixed << setprecision(2) << throughput << " ops/sec" << endl;
    cout << "Average Latency:  " << fixed << setprecision(3) << avg_latency_us << " µs" << endl;
    cout << "Min Latency:      " << fixed << setprecision(3) << stats.min_latency_us << " µs" << endl;
    cout << "P50 Latency:      " << fixed << setprecision(3) << p50 << " µs" << endl;
    cout << "P99 Latency:      " << fixed << setprecision(3) << p99 << " µs" << endl;
    cout << "Max Latency:      " << fixed << setprecision(3) << stats.max_latency_us << " µs" << endl;
}

int main() {
    cout << "Custom KV Engine Benchmark" << endl;
    cout << "=================================================" << endl;
    cout << "Total Operations:   " << NUM_OPERATIONS << endl;
    cout << "Concurrent Clients: " << CONCURRENT_CLIENTS << endl;

    int operations_per_client = NUM_OPERATIONS / CONCURRENT_CLIENTS;

    cout << "\n[1/2] Benchmarking SET operations..." << flush;
    auto start_set = chrono::high_resolution_clock::now();
    vector<thread> threads;
    for (int i = 0; i < CONCURRENT_CLIENTS; ++i) {
        threads.emplace_back(benchmarkSet, i, operations_per_client);
    }
    for (auto& t : threads) t.join();
    auto end_set = chrono::high_resolution_clock::now();
    cout << " Done in " << chrono::duration_cast<chrono::milliseconds>(end_set - start_set).count() << " ms" << endl;

    threads.clear();

    // return 1;

    cout << "[2/2] Benchmarking GET operations..." << flush;
    auto start_get = chrono::high_resolution_clock::now();
    for (int i = 0; i < CONCURRENT_CLIENTS; ++i) {
        threads.emplace_back(benchmarkGet, i, operations_per_client);
    }
    for (auto& t : threads) t.join();
    auto end_get = chrono::high_resolution_clock::now();
    cout << " Done in " << chrono::duration_cast<chrono::milliseconds>(end_get - start_get).count() << " ms" << endl;
    
    printStats("SET Operations", set_stats);
    printStats("GET Operations", get_stats);

    return 0;
}