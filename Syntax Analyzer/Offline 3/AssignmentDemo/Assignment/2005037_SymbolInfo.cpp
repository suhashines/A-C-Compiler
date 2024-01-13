#include<string>

using namespace std;

class SymbolInfo{
    string name ;
    string type ;

    public:

    SymbolInfo* next ;
    bool isFunction ;

    SymbolInfo(const string &n,const string &t,SymbolInfo* next=nullptr){
        name = n ;
        type = t ;
        this->next = next ;
        isFunction = false;
    }

    string getName(){
        return name ;
    }
    string getType(){
        return type ;
    }
};