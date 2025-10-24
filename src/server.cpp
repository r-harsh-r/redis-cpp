#include "../include/server.h"
#include "../include/ioFull.h"

using namespace std;

SERVER::SERVER(std::string&PORT_){
    PORT = PORT_;
    sockfd = -1;
}

int SERVER::startServer(int BACKLOG ){
    struct addrinfo hints;
    struct addrinfo * res;

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL,PORT.c_str(),&hints,&res);

    if(status != 0){
        fprintf(stderr , "gai err : %s\n",gai_strerror(status));
        exit(1);
    }

    int flag = 0;
    for(struct addrinfo * ptr = res;ptr != NULL;ptr = ptr->ai_next){
        sockfd = socket(res->ai_family,res->ai_socktype , res->ai_protocol);

        if(sockfd == -1){
            continue;
        }

        if(bind(sockfd, res->ai_addr,res->ai_addrlen) == -1){
            close(sockfd);
            continue;
        }
        flag = 1;
        break;
    }

    freeaddrinfo(res);

    if(flag == 0){
        cout<<"Connection could not be established...\n";
        return -1;
    }

    listen(sockfd,BACKLOG);
 
    cout<<"Socket listening at port : "<<PORT<<" ..."<<endl;
    return sockfd; 
}

void SERVER::runServer(){
    struct sockaddr_storage their_addr;
    socklen_t sin_size = sizeof their_addr; 
    
    while(true){
        int clientFD = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size);
    
        // handle this client in new thread
        thread t(&SERVER::handleConnection ,this , clientFD);
        t.detach();
    }

    close(sockfd);

}

void SERVER::handleConnection(int clientFD){
    while(true){
        // read full request
        string req_serialized = readFull(clientFD);

        if(req_serialized == ""){
            cout<<"Connection Closed\n";
            break;
        }

        string req = resp.deSerialize(req_serialized);

        // parse this request and get the res
        // use data structure here to store fetch etc

        // write full response
        string res = "OK";
        string res_serialized = resp.serialize(res);
        writeFull(clientFD, res_serialized);
    }

    close(clientFD);

}
