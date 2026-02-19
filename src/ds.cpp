#include "../include/ds.h"

void DS::set(string&key,string&val,time_t del){
    std::lock_guard<std::mutex> lock(mtx);
    // cout<<"Received : "<<key<<"::"<<val<<endl;
    if(del == -1){
        // timeLimit[key] = del; 
        timeLimit.insert(key,del); 
    }else{
        // timeLimit[key] = time(0) + del;
        time_t delPlusTime = del + time(0);
        timeLimit.insert(key,delPlusTime); 
    }

    // mp[key] = val;
    mp.insert(key,val);
    // mp.insert
}

string DS::get(string&key){
    std::lock_guard<std::mutex> lock(mtx);
    string empty = "";
    if(timeLimit.find(key) == -1) return mp.find(key);
    if(time(0) > timeLimit.find(key)) mp.insert(key,empty);
    return mp.find(key);
}