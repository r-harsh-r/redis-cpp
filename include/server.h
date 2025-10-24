#pragma once
#include<string>
#include<iostream>
#include<sys/types.h>
#include<cstring>
#include<sys/socket.h>
#include<netdb.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<thread>
#include<vector>
using namespace std;

#include "../include/RESP.h"
#include "../include/ds.h"

class SERVER{
    std::string PORT;
    int sockfd;
    RESP resp;
    DS ds;

public:
    SERVER(std::string&);
    int startServer(int BACKLOG= 10);
    void runServer();
    void handleConnection(int);
    string handleRequest(string&);
};
