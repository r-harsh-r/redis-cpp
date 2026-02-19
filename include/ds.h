#include<string>
#include<unordered_map>
#include<ctime>
#include<mutex>
#include "../hashtable/hashtable.h"
using namespace std;

class DS{
    // unordered_map<string,string>mp;
    // HashTable mp;
    // unordered_map<string,time_t>timeLimit;

    HashTable<string,string>mp;
    HashTable<string,time_t>timeLimit;
    std::mutex mtx;

public:
    void set(string&,string&,time_t = -1);
    string get(string&);
};