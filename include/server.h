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

#include "./RESP.h"

class SERVER{
    std::string PORT;
    int sockfd;
    RESP resp;

public:
    SERVER(std::string&);
    int startServer(int BACKLOG= 10);
    void runServer();
    void handleConnection(int);

};
