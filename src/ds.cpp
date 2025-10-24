#include "../include/ds.h"

void DS::set(string&key,string&val,time_t del){
    if(del == -1){
        timeLimit[key] = del; 
    }else{
        timeLimit[key] = time(0) + del;
    }

    mp[key] = val;
}

string DS::get(string&key){
    if(timeLimit[key] == -1) return mp[key];
    if(time(0) > timeLimit[key]) mp[key] = "";
    return mp[key];
}