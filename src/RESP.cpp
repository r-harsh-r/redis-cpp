#include "../include/RESP.h"
#include<algorithm>

std::string RESP::serialize(std::string &str){
    int length = (int)str.length();

    std::string str_serialized = "$" + std::to_string(length) + "\r\n" + str + "\r\n";

    return str_serialized;
}

std::string RESP::deSerialize(std::string&str){
    // int totLen = str.length();

    std::string recovered = "";

    int idx = 0;
    while(str[idx] != '\r') idx++;
    
    // extract the length of the string
    std::string length = "";
    for(int i = 1;i<idx;i++){
        length += str[i];
    }
    // std::reverse(length.begin(),length.end());
    int len = stoi(length);
    
    idx = idx + 2;

    while(len--){
        recovered += str[idx++];
    }


    return recovered;
}
