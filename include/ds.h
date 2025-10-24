#include<string>
#include<unordered_map>
#include<ctime>
using namespace std;

class DS{
    unordered_map<string,string>mp;
    unordered_map<string,time_t>timeLimit;

public:
    void set(string&,string&,time_t = -1);
    string get(string&);
};