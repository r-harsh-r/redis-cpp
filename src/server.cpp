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


vector<string> parse(string&req){
    vector<string>ans;

    int n = req.size();

    int tail = 0;
    int head = tail - 1;
    string temp = "";
    while(tail < n){
        while(head + 1 < n && req[tail] != ' ' && req[head + 1] != ' '){
            head++;
            temp += req[head];
        }

        if(tail <= head){
            ans.push_back(temp);
            tail = head + 1;
        }else{
            tail++;
            head = tail - 1;
        }
        temp = "";
    }

    return ans;

}

string SERVER::handleRequest(string&req){
    vector<string>fragmented = parse(req);


    // processing the request - business logic
    string res = "";
    
    if(fragmented[0] == "PING") res = "PONG";
    
    if(fragmented[0] == "ECHO") {
        for(int i = 1;i < (int)fragmented.size();i++){
            res += fragmented[i]+ " ";
        }
    }

    if(fragmented[0] == "SET"){
        if(fragmented.size() == 5){
            if(fragmented[3] == "EX"){
                ds.set(fragmented[1],fragmented[2],stoi(fragmented[4]));
            }else return "ERROR";
        }else{
            ds.set(fragmented[1] , fragmented[2]);
        }
        return "OK";
    }
    
    if(fragmented[0] == "GET"){
        return ds.get(fragmented[1]);
    }

    return res;

}

void SERVER::handleConnection(int clientFD){
    cout<<"Connection opened"<<endl;
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
        string res = handleRequest(req);

        string res_serialized = resp.serialize(res);
        writeFull(clientFD, res_serialized);
    }

    close(clientFD);

}
