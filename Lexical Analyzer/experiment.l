%option noyywrap

%{

#include<iostream>
#include<fstream>
using namespace std;

%}

%%

"==" {cout<<"relop found"<<endl;}
"="  {cout<<"equ found"<<endl;}
[ \t\v] {cout<<"whitespace found"<<endl;}

%%

int main(int argc,char *[]argv){

    FILE* fin = fopen(argv[1],'r');
    yyin = fin ;
    yylex();
    fclose(fin);
    return 0 ;

}