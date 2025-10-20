#include "../include/ioFull.h"

string readFull(int sockFD){
    string str = "";

    char ch[1];

    // listen till \r\n
    while(true){
        int byteRead = recv(sockFD,ch,1,0);

        if(byteRead <= 0){
            return str;
            break;
        }

        str.push_back(ch[0]);
        
        if(str.size() >= 2 && str[str.size() - 1] == '\n' && str[str.size() - 2] == '\r') break;
    }
    
    // extract the byte size
    string len_str = "";
    for(int i = 1;i<(int)str.size() - 2;i++){
        len_str += str[i];
    }
    int len = stoi(len_str);

    // read for len+2
    for(int i = 0;i<len + 2;i++){
        int byteRead = recv(sockFD , ch , 1 , 0);
        
        if(byteRead <= 0){
            break;
        }
    
        str.push_back(ch[0]);
    }

    return str;   
}

void writeFull(int sockFD,string&str){
    char ch[1];
    int totLen = str.length();
    
    for(int i = 0;i<totLen;i++){
        ch[0] = str[i];

        int byteSent = send(sockFD, ch , 1, 0);

        if(byteSent < 1){
            cout<<"Sending error...\n";
            return;
        }
    }
}