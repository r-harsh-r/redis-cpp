// change the way server handle connection to incorporate data structure

#include "../include/RESP.h"
#include "../include/server.h"
#include "../include/client.h"
#include<iostream>

using namespace std;

int main(int argc , char*argv[]){
    if(argc < 2){
        cout<<"Invalid command"<<endl;
        return 1;
    }
    
    // server
    if(string(argv[1]) == "-s"){
        if(argc < 3){
            cout<<"Invalid command : enter PORT number"<<endl;
            return 1;
        }
        
        string PORT = string(argv[2]);
        SERVER server(PORT);
        
        int sockfd = server.startServer();

        if(sockfd < 0){
            cout<<"Error initiating the socket\n";
            return 1;
        }

        server.runServer();
    }
    else if(string(argv[1]) == "-c"){
        if(argc < 4){
            cout<<"Invalid command : enter PORT number"<<endl;
            return 1;
        }

        string URL = string(argv[2]);
        string PORT = string(argv[3]);
        
        CLIENT client(URL,PORT);

        client.connectServer();

        client.runClient();
    }

    return 1;
}