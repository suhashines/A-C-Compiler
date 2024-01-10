%{

#include<bits/stdc++.h>
#include<cstring>
#include<string>
#include "y.tab.h"

using namespace std;
int lineCount = 1 ;

%}

%%

[\n\r] {lineCount++ ;
return NEWLINE ;
}

[0-9]+ {
    yylval.intValue = atoi(yytext);
    return NUMBER ;
}

[A-Za-z_]+ {
    yylval.stringValue = strdup(yytext) ;
    return ALPHABET;}

%%

int yywrap(void){
    return 1 ;
}