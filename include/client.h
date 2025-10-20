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

using namespace std;
#include "./RESP.h"
#include "./ioFull.h"

class CLIENT{
    string serverURL;
    string serverPORT;
    int serverFD;
    RESP resp;

public:
    CLIENT(string& , string&);
    void connectServer();
    void runClient();
};