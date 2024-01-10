#include<string>

using namespace std;

class SymbolInfo{
    string name ;
    string type ;

    public:

    SymbolInfo* next ;

    SymbolInfo(const string &n,const string &t,SymbolInfo* next=nullptr){
        name = n ;
        type = t ;
        this->next = next ;
    }

    string getName(){
        return name ;
    }
    string getType(){
        return type ;
    }
};