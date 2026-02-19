#include <iostream>
#include <stdexcept>
#include "../include/ioFull.h"

using namespace std;

string readFull(int sockFD){
    string str = "";
    char ch[1];

    // reading header
    while(true){
        int byteRead = recv(sockFD,ch,1,0);
        if(byteRead <= 0){
            return ""; 
        }
        str.push_back(ch[0]);
        if(str.size() >= 2 && str.back() == '\n' && str[str.size() - 2] == '\r') break;
    }
    
    if(str.size() < 3) return ""; 

    // extract the byte size
    string len_str = "";
    // skipping the first char ($) and last 2 (\r\n)
    for(int i = 1; i < (int)str.size() - 2; i++){
        len_str += str[i];
    }
    
    int len = 0;
    try {
        if(!len_str.empty()) {
            len = stoi(len_str);
        } else {
            return ""; 
        }
    } catch (const std::exception& e) {
        cerr << "Error parsing RESP length: " << len_str << endl;
        return ""; 
    }

    // read for len+2 (payload + \r\n)
    for(int i = 0; i < len + 2; i++){
        int byteRead = recv(sockFD , ch , 1 , 0);
        if(byteRead <= 0){
            return ""; // Connection dropped mid-packet
        }
        str.push_back(ch[0]);
    }

    return str;   
}

void writeFull(int sockFD,string&str){
    char ch[1];
    int totLen = str.length();

    int totSent = 0;
    int byteLeft = totLen;
    const char*buff = str.c_str();

    while(totSent < byteLeft){
        int n = send(sockFD,buff + totSent,byteLeft - totSent,0);

        if(n == -1){
            cout<<"Sending error...\n";
            return;
        }
        totSent += n;
    }
}