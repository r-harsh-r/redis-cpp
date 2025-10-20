#pragma once
#include<iostream>
#include<sys/socket.h>
using namespace std;

string readFull(int);
void writeFull(int , string&);