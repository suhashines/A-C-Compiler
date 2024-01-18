#include<string>
#include<vector>

using namespace std;

class SymbolInfo{
    string name ;
    string type ;

    public:

    SymbolInfo* next ;
    bool isFunction ;
    string dataType ;

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




class FunctionInfo : public SymbolInfo {

   
   vector<string>parametreList ;
   string returnType ;

   public: 
        bool isDefined ;

        FunctionInfo(const string &name,const string &type): SymbolInfo(name,type){
            isFunction = true;
            isDefined = false;
        }

        void setReturnType(string returnType){
            this->returnType = returnType;
            dataType = returnType ;
        }

        string getReturnType(){
            return returnType;
        }

        void addParameter(const string &name){
            parametreList.push_back(name);
        }

        int getNumberOfParameters(){
            return parametreList.size();
        }

        string findParamAtIndex(int index){
            if(index>=parametreList.size()) return "NONE" ;

            return parametreList[index];
        }
      
};



class IdInfo : public SymbolInfo {

public:
 string idType ;
 int size ;
 IdInfo(const string&name, const string &type):SymbolInfo(name,"ID"){
    idType = type ;
    dataType = type ;
    size = -1 ;
 }

 IdInfo(const string&name,const string&type,int sz):SymbolInfo(name,"ID"){
    idType = type;
    size = sz;
    dataType = type ;
 }


};