%{
#include<iostream>
#include<string>
using namespace std;

extern int lineCount ;
extern FILE * yyin;


int yyparse(void);
int yylex(void);

void yyerror(const string&error){

}

%}

%union {
    int intValue ;
    char* stringValue ;
}

%token <intValue> NUMBER 
%token NEWLINE 
%token <stringValue> ALPHABET

%%

program : lines

lines : lines line
      | line

line : expr NEWLINE

expr : NUMBER {cout<<"Line : "<<lineCount<<"# number "<<$1<<" found\n";}
     | ALPHABET {cout<<"Line : "<<lineCount<<"# alphabets "<<$1<<" found\n";}
     | NEWLINE 
%%

int main(){

    FILE *fp ;
    fp = fopen("input.txt", "r");

    yyin = fp ;
    yyparse();
    fclose(fp);
}
