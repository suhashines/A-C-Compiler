#include "2005037_SymbolInfo.cpp"

static unsigned long long sdbm(unsigned char *str)
{
    unsigned long long hash = 0;
    int c;

    while (c = *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

class ScopeTable
{

public:
    ScopeTable *parent;

    string id;
    int suffix;
    unsigned long long bucket;

    // parent idicates the previous scope table
    // every scope table has its own id that consists of parent's id+suffix

    SymbolInfo **arr;

    ScopeTable(unsigned long n,ScopeTable* par=nullptr)
    {

        bucket = n;
        parent = par;
        suffix = 0 ;
        setId() ;

        arr = new SymbolInfo *[bucket];

        for (int i = 0; i < bucket; i++)
        {
            arr[i] = nullptr;
        }
    }


    void setId(){
        
        if(parent==nullptr){
            id = "1" ;
            return ;
        }
        int linearId = stoi(parent->id) + (++parent->suffix) ;
        id = to_string(linearId);
        
        //id = parent->id + "." + to_string(++parent->suffix);
    
    }

    

    int hashFunction(const string &name)
    {

        unsigned long long hashValue = sdbm((unsigned char *)name.c_str());
        return hashValue % bucket;
    }

    SymbolInfo *lookup(const string &name)
    {
        int index = hashFunction(name);

        SymbolInfo *curr = arr[index];

        int chain=1 ;

        while (curr != nullptr)
        {

            if (curr->getName() == name)
            {   //cout<<"'"<<name<<"' found at position <"<<index+1<<", "<<chain<<"> of ScopeTable# "<<id<<endl;
                return curr;
            }

            curr = curr->next;
            chain++ ;
        }

        return nullptr;
    }

    SymbolInfo* insertUtil(const string&name){

         int index = hashFunction(name);

        SymbolInfo *curr = arr[index];

        while (curr != nullptr)
        {

            if (curr->getName() == name)
            { 
                return curr;
            }

            curr = curr->next;
        
        }

        return nullptr;
    }

    

    bool insert(const string &name, const string &type,const string &idType,int size,bool isGlobal,int offset)
    {   //cout<<"\t" ;

        SymbolInfo *symbol = insertUtil(name);

        if (symbol != nullptr)
          {  //cout<<"'"<<name<<"'"<<" already exists in the current ScopeTable# "<<id<<endl;
             return false;
             }

        symbol = new IdInfo(name, idType,size,isGlobal,offset);

        int index = hashFunction(name);
        int chain = 1 ;

        if (arr[index] == nullptr)
        {
            arr[index] = symbol;
            //cout<<"Inserted  at position "<<"<"<<index+1<<", "<<chain<<"> of ScopeTable# "<<id<<endl;
            return true;
        }

        chain ++ ;

        SymbolInfo *curr = arr[index];

        while (curr->next != nullptr)
        {
            curr = curr->next;
            chain++ ;
        }

        curr->next = symbol;

        //cout<<"Inserted  at position "<<"<"<<index+1<<", "<<chain<<"> of ScopeTable# "<<id<<endl;

        return true;
    }

    bool insert(const string &name, const string &type,bool isFunction=false)
    {   //cout<<"\t" ;

        SymbolInfo *symbol = insertUtil(name);

        if (symbol != nullptr)
          {  //cout<<"'"<<name<<"'"<<" already exists in the current ScopeTable# "<<id<<endl;
             return false;
             }

     
        symbol = new FunctionInfo(name, type);  

        

        int index = hashFunction(name);
        int chain = 1 ;

        if (arr[index] == nullptr)
        {
            arr[index] = symbol;
            //cout<<"Inserted  at position "<<"<"<<index+1<<", "<<chain<<"> of ScopeTable# "<<id<<endl;
            return true;
        }

        chain ++ ;

        SymbolInfo *curr = arr[index];

        while (curr->next != nullptr)
        {
            curr = curr->next;
            chain++ ;
        }

        curr->next = symbol;

        //cout<<"Inserted  at position "<<"<"<<index+1<<", "<<chain<<"> of ScopeTable# "<<id<<endl;

        return true;
    }

    void printTable()
    {   //cout<<"\tScopeTable# "<<id<<endl;

        for (int i = 0; i < bucket; i++)
        {
            //cout<<"\t"<<i+1 ;

            SymbolInfo *curr = arr[i];

            if (curr == nullptr)
            {   //cout<<endl;
                continue;
            }

            while (curr != nullptr)
            {
                //cout <<" --> "<< "(" << curr->getName() << "," << curr->getType() << ")";
                curr = curr->next;
            }

            //cout << endl;
        }

    }


std::string getTableString()
{
    std::string tableString;

    tableString += "\tScopeTable# " + id + "\n";

    for (int i = 0; i < bucket; i++)
    {
        
        SymbolInfo *curr = arr[i];

        if (curr == nullptr)
        {
            // tableString += "\n";
            continue;
        }

        tableString += "\t" + std::to_string(i + 1)+"--> ";

        while (curr != nullptr)
        {     tableString += "<" + curr->getName() + "," ;

              if(curr->isFunction){

               FunctionInfo * currFunc = (FunctionInfo*)curr;
              
              tableString += "FUNCTION,"+currFunc->getReturnType();
              
              }else{
                IdInfo* currId = (IdInfo*)curr;
                if(currId->size!=-1){
                    //this is an array
                    tableString+= "ARRAY";
                }else{
                    //this is an identifier
                    tableString += currId->idType ;
                }
              }
              tableString += "> ";
              curr = curr->next;
        }
    
            tableString += "\n";
        }

    return tableString;
}


    SymbolInfo *removeUtil(const string &name)
    {

        int index = hashFunction(name);

        if (arr[index] == nullptr){

        //cout << "Not found in the current ScopeTable# "<<id<< endl;
        return nullptr;
        }
        
        int chain = 1;

        if(arr[index]->getName() == name){

           // cout<<"Deleted "<<"'"<<name<<"'"<<" from position <"<<index+1<<", "<<chain<<"> of ScopeTable# "<<id<<endl;
            SymbolInfo *temp = arr[index];
            arr[index] = arr[index]->next;
            return temp;
        }

        SymbolInfo* curr = arr[index]->next;
        SymbolInfo* prev = arr[index];

        chain++ ;

        while(curr!=nullptr){

            if(curr->getName() == name){
                prev->next = curr->next;
                //cout<<"Deleted '"<<name<<"' from position <"<<index+1<<", "<<chain<<"> of ScopeTable# "<<id<<endl;
                return curr;
            }
            curr = curr->next;
            prev = curr ;
            chain++ ;
        }

         //cout << "Not found in the current ScopeTable# " << endl;

         return nullptr;
    }

    bool remove(const string &name){

        SymbolInfo *symbol = removeUtil(name);

        if(symbol==nullptr){
            return false;
        }

        else{
            delete symbol;
            return true;
        }
    }

    ~ScopeTable(){

        for (int i = 0; i < bucket; i++)
        {
            SymbolInfo *curr = arr[i];
            while (curr != nullptr)
            {
                SymbolInfo *temp = curr;
                curr = curr->next;
                delete temp;
            }
            arr[i] = nullptr;
        }

        delete[] arr;
    }

};
