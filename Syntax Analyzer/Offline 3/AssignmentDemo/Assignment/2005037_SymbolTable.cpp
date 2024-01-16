#include <iostream>
#include <list>
#include "2005037_ScopeTable.cpp"


using namespace std;

class SymbolTable
{

    ScopeTable *head;
    ScopeTable *tail;
    int bucket;

    list <FunctionInfo*> functionList ;

public:
    SymbolTable(int n)
    {
        bucket = n;
        head = new ScopeTable(bucket);
        // cout<<"\tScopeTable# "<<head->id<<" created"<<endl;
        tail = head;
    }

    void enterScope()
    {

        ScopeTable *temp = new ScopeTable(bucket, tail);

        // cout<<"\tScopeTable# "<<temp->id<<" created"<<endl;

        tail = temp;
    }

    void exitScope()
    {   // cout<<"\t";
        string str = "\t";

        if (head == tail)
        {   //cout<<"ScopeTable# "<<tail->id<<" cannot be deleted"<<endl;
            str += "ScopeTable# "+tail->id+" cannot be deleted\n";
            return ;
        }

        ScopeTable *temp = tail;
        tail = tail->parent;
        //cout<<"ScopeTable# "<<temp->id<<" deleted"<<endl;
        delete temp;
    }

    bool insert(const string &name,const string &type,const string &idType)
    {
        return tail->insert(name, type,idType);
    }

    bool insert(const string &name,const string &type,bool isFunction=false){

        if(tail->insert(name,type,isFunction)){
            functionList.push_back((FunctionInfo*)tail->lookup(name));
            return true;
        }

        return false;
    }

    bool containsFunction(const string &name){

        for(FunctionInfo* function: functionList){
            if(function->getName()==name){
                return true;
            }
        }

        return false;
    }

    bool remove(const string &name)
    {
        return tail->remove(name);
    }

    SymbolInfo *lookup(const string &name)
    {   //cout<<"\t" ;

        ScopeTable *curr = tail;

        while (curr != nullptr)
        {
            SymbolInfo *symbol = curr->lookup(name);

            if (symbol != nullptr)
                return symbol;
            curr = curr->parent;
        }

        //cout<<"'"<<name<<"'"<<" not found in any of the ScopeTables"<<endl;

        return nullptr ;
    }

    void printCurrent(){
        tail->printTable();
    }


    std::string printAll(){

        std::string temp ;

        ScopeTable* curr = tail ;

        while (curr!= nullptr)
        {
            temp += curr->getTableString();
            curr = curr->parent;
        }
        
        return temp ;
    }

    ~SymbolTable(){

       while(head!=tail){
        exitScope();
       }

    //    cout<<"\tScopeTable# "<<head->id<<" deleted"<<endl;
       
       delete head;
    }

    
};