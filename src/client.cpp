#include "../include/client.h"

CLIENT::CLIENT(string&url,string&port){
    serverURL = url;
    serverPORT = port;
}

void CLIENT::connectServer(){
    int status;
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(serverURL.c_str() , serverPORT.c_str(),&hints,&res);

    if(status != 0){
        fprintf(stderr , "gai error in client : %s\n",gai_strerror(status));
        exit(1);
    }

    // loop through all the result and connect to the first valid one
    struct addrinfo*ptr;
    for(ptr = res;ptr!= NULL;ptr = ptr->ai_next){
        serverFD = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
        if(serverFD == -1){
            // perror("\tclient connect error\n");
            continue;
        }

        char * ip4 = (res->ai_family == AF_INET) ? new char[INET_ADDRSTRLEN] : new char[INET6_ADDRSTRLEN];

        if(res->ai_family == AF_INET){
            sockaddr_in * sa = (sockaddr_in*)(res->ai_addr);

            inet_ntop(res->ai_family , &(sa->sin_addr),ip4,INET_ADDRSTRLEN);

            cout<<"Connecting with address : "<<ip4<<endl;
        }else{
            sockaddr_in6 * sa = (sockaddr_in6*)(res->ai_addr);

            inet_ntop(res->ai_family , &(sa->sin6_addr),ip4,INET6_ADDRSTRLEN);

            cout<<"Connecting with address : "<<ip4<<endl;
        }


        if(connect(serverFD,res->ai_addr,res->ai_addrlen) == -1){
            perror("client : connect error\n");
            close(serverFD);
            continue;
        }

        break;
    }

    if(ptr == NULL){
        cout<<"No server found"<<endl;
        serverFD = -1;
        return ;
    }

    freeaddrinfo(res);
}

void CLIENT::runClient(){
    while(true){
        // get request
        string req;
        getline(cin,req);

        // serialise the request
        string serialized_req = resp.serialize(req);

        // send req
        writeFull(serverFD,serialized_req);

        // get response
        string serialized_res;
        serialized_res = readFull(serverFD);

        // deserialise
        string res;
        res = resp.deSerialize(serialized_res);

        // print
        cout<<"\t"<<res<<endl;
    }

    close(serverFD);
}