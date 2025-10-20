#pragma once
#include<string>

class RESP{
public:
    std::string serialize(std::string&); 
    std::string deSerialize(std::string&);    
};