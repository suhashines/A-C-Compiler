#include<string>
#include<vector>
#include "2005037_SymbolInfo.cpp"

using namespace std; 

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