# In-Memory Key-Value Engine

A lightweight, custom implementation of a Redis-like key-value store in C++. This project demonstrates a multi-threaded server architecture, custom hash table implementation with progressive rehashing, and a simplified RESP (Redis Serialization Protocol) for client-server communication.

## Features

- **In-Memory Key-Value Store**: Fast SET and GET operations.
- **Custom Hash Table**: 
  - Implemented from scratch (no `std::unordered_map` for core storage).
  - **Progressive Rehashing**: Resizes the hash table incrementally to avoid latency spikes during growth.
- **Thread-Safe Architecture**: Uses `std::mutex` to protect shared data structures in a multi-threaded environment.
- **Client-Server Model**:
  - **Server**: Handles multiple concurrent client connections using threads.
  - **Client**: Interactive CLI to communicate with the server.
- **RESP Protocol**: Implements a simplified version of the Redis Serialization Protocol for command and data transmission.
- **TTL Support**: `SET` commands support an optional expiration time (`EX` flag)

## Project Structure

```
redis-cpp/
├── benchmark.cpp       # Performance benchmarking tool
├── hashtable/
│   └── hashtable.h     # Custom Hash Table
├── include/            # Header files
│   ├── client.h
│   ├── ds.h            # Data Store wrapper
│   ├── ioFull.h        # Network I/O helpers
│   ├── RESP.h          # Protocol serializer/deserializer
│   └── server.h
├── src/                # Source files
│   ├── client.cpp
│   ├── ds.cpp
│   ├── ioFull.cpp
│   ├── main.cpp        # Entry point
│   ├── RESP.cpp
│   └── server.cpp
└── Makefile            # Build configuration
```

## Building

The project uses a `Makefile` for compilation. Ensure you have `g++` installed.

### Build Server and Client
```bash
make
# or
make all
```

### Build Benchmark Tool
```bash
make benchmark
```

### Clean Build Artifacts
```bash
make clean
```

## Usage

### 1. Start the Server
Run the server on a specific port (e.g., 6379).
```bash
./redis_server -s 6379
```

### 2. Start the Client
Connect to the server using the client CLI. Provide the hostname and port.
```bash
./redis_server -c 127.0.0.1 6379
```

### 3. Client Commands
Once connected, you can issue commands:
- **SET**: Store a key-value pair.
  ```
  >>> SET mykey myvalue
  ```
- **SET with Expiry**: Store a key with a Time-To-Live (seconds).
  ```
  >>> SET mykey myvalue EX 10
  ```
- **GET**: Retrieve a value by key.
  ```
  >>> GET mykey
  ```
- **PING**: Check connectivity.
  ```
  >>> PING
  ```
- **ECHO**: Echo back a string.
  ```
  >>> ECHO hello world
  ```

## Benchmarking

To test the performance of the server, first ensure the server is running, then run the benchmark tool.

```bash
./benchmark
```
*Note: The benchmark tool is configured in `benchmark.cpp`. You can adjust parameters like `NUM_OPERATIONS` and `CONCURRENT_CLIENTS` in the source file.*

## Implementation Details

### Hash Table
The core data structure is a custom Hash Table located in `hashtable/hashtable.h`. It handles collisions using chaining (linked lists) and performs **progressive rehashing** when the load factor exceeds a threshold. This ensures that resizing the table doesn't block the server for a long duration, maintaining low latency.

### Protocol (RESP)
Communication follows a simplified Redis Serialization Protocol:
- Commands are serialized as Bulk Strings (e.g., `$3\r\nSET\r\n`).
- The server parses these strings to execute commands.

## Requirements
- Linux/Unix environment (uses POSIX socket headers)
- C++11 compliant compiler (g++)
- Make
