#include <iostream>
#include<sstream>
#include "2005037_SymbolTable.cpp"

using namespace std;

string arr[10] ;
int len ;

void processLine(const std::string& line) {
    
    for (int i = 0; i < 10; ++i) {
        arr[i] = "";
    }
    len = 0;

    std::istringstream iss(line);
    std::string word;

    while (iss >> word && len < 10) {
        arr[len++] = word;
    }

    len-- ;
}


int main()
{

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    int n;
    int count = 1;
    string line;
    string cmd ;

    cin>>n ;

    getline(cin, line);

    SymbolTable table(n);

    string msg = "Wrong number of arugments for the command" ;

    do
    {
        getline(cin,line);

        cout<<"Cmd "<<count<<": "<<line<<endl;

        processLine(line);

        cmd = arr[0] ;

        if (cmd == "I")
        {   
            if(len!=2){
                cout<<"\t"<<msg<<" I"<<endl;
                count++;
                continue;
            }
            string name = arr[1];
            string type = arr[2] ;

            table.insert(name, type);
        }
        else if (cmd == "P")
        {   
             if(len!=1){
                cout<<"\t"<<msg<<" P"<<endl;
                count++;
                continue;
            }

            string option = arr[1];
            

            if (option == "C")
            {
                table.printCurrent();
            }
            else if (option == "A")
            {
                table.printAll();
            }else{
                cout<<"\tInvalid argument for the command P"<<endl;
            }
        }
        else if (cmd == "D")
        {   if(len!=1){
            cout<<"\t"<<msg<<" D"<<endl;
            count++;
            continue;
        }
            string name = arr[1];
            
            table.remove(name);
        }
        else if (cmd == "L")
        {   if(len!=1){
                cout<<"\t"<<msg<<" L"<<endl;
                count++;
                continue;
            }
            string name = arr[1];
            
            table.lookup(name);
        }
        else if (cmd == "S")
        {    if(len!=0){
                cout<<"\t"<<msg<<" S"<<endl;
                count++;
                continue;
            }

            table.enterScope();
        }
        else if (cmd == "E")
        {   if(len!=0){
                cout<<"\t"<<msg<<" E"<<endl;
                count++;
                continue;
            }

            table.exitScope();
        }

        count++ ;

    } while (cmd!="Q");
}