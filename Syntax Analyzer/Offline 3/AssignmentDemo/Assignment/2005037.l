%option noyywrap
%x CHAR_STATE
%x STRING_STATE
%x SINGLE_LINE_COMMENT_STATE
%x MULTILINE_COMMENT_STATE
%x NEWLINE_STATE

%{

#include<iostream>
#include<fstream>
#include<string>
#include<cctype>
#include "2005037_SymbolInfo.cpp"
#include "y.tab.h"

using namespace std;

extern YYSTYPE yylval ;

int line = 1 ;
int error = 0 ;
int warning = 0 ;
int globalIndent = 0 ;
int localIndent ;
int tempLine;
bool isMultiLineString = false;

extern ofstream logFile;
extern ofstream errorFile;


ofstream tokenFile;

string characterLiteral;
string lexeme ;

//SymbolTable table(10);

string toUpper(const string &str){

    string upper ;

    for(char ch:str){
        upper += toupper(ch);
    }

    return upper;

}

char extractChar(const string &charLiteral) {
    if (charLiteral.length() == 1) {
        return charLiteral[0];
    } else if (charLiteral.length() == 2) {
        switch (charLiteral[1]) {
            case 'a': return '\a'; 
            case 'b': return '\b'; 
            case 'f': return '\f'; 
            case 'n': return '\n'; 
            case 'r': return '\r'; 
            case 't': return '\t'; 
            case 'v': return '\v'; 
            case '\\': return '\\'; 
            case '\'': return '\''; 
            case '\"': return '\"'; 
            case '0': return '\0';  
           
            default: return -1; // Unknown escape sequence
        }
    }

    
    return -1;
}


void lexLogWriter(const string &token,const string &lexeme){
   
       logFile<<"Line# "<<line<<": Token <"<<token<<"> Lexeme "<<lexeme<< " found"<<endl; 
    }

void tokenFileWriter(const string& token,const string &lexeme){
    //tokenFile<<"<"<<token<<", "<<lexeme<<">\n" ;
}

void errorWriter(const string &errorType,const string&lexeme){
    error ++ ;
    logFile<<"Error at line# "<<line<<": "<<errorType<<" "<<lexeme<<endl;
}

void lexLogWriter(const string &token,const string &lexeme,int lineCount){

    logFile<<"Line# "<<lineCount<<": Token <"<<token<<"> Lexeme "<<lexeme<< " found"<<endl; 
}

void warningWriter(int tab,int indent){
    logFile<<"Line# "<<line<<": warning, "<<indent<<" of tabs needed but got "<<tab<<" tabs."<<endl;
}

void insertSymbol(const string &token,const string &lexeme){

    // if(table.insert(lexeme,token)){
    //     logFile<<table.printAll();
    // }else{
    //     logFile<<"\t"<<lexeme<<" already exists in the current ScopeTable\n" ;
    // }
}

void endOfFile(){
    //logFile<<table.printAll();
}



%}

UNIX        [\n]
WINDOWS     [\r\n]
NEWLINE     {UNIX}|{WINDOWS}
WHITESPACE  [ \t\v\f\r]

DIGIT       [0-9]
INT         {DIGIT}+
FLOAT       (({INT}("."{INT})?)|("."{INT}))(([eE][+-]?)?{INT})?
ID          [a-zA-Z_][a-zA-Z0-9_]*
ADDOP       [+-]
MULOP       [*/%]
INCOP       "++"
DECOP        "--"
RELOP       "<"|"<="|">"|">="|"=="|"!="
ASSIGNOP    "="
LOGICOP     "&&"|"||"
BITOP       "&"|"|"|"^"|"<<"|">>"
NOT         !

LPAREN      "("
RPAREN       ")"

LCURL      "{"
RCURL      "}"

LSQUARE    "["
RSQUARE    "]"

COMMA     ","
SEMICOLON  ";"

TOO_MANY_DECIMAL_POINTS   ({INT}?\.)+{FLOAT}

ILLFORMED_NUMBER          (({INT}(.{INT})?)|(.{INT}?))([eE][+-]?{DIGIT}*"."{DIGIT}*)

INVALID_ID_SUFFIX_NUM_PREFIX  {FLOAT}{ID}+

%%

{NEWLINE}  {   line++ ;
               BEGIN NEWLINE_STATE;
}

<NEWLINE_STATE>^[\t]*{NEWLINE}  {line++ ;}

<NEWLINE_STATE>^[\t]*[^ ] {
            
            //cout<<"rule 2 at line "<<line<<" global: "<<globalIndent<<" read: "<<yytext[yyleng-1]<<endl;

            int tab = yyleng-1;
            int indent = globalIndent; 

            if(yytext[yyleng-1]=='}' && globalIndent!=0){
                indent -- ;
            }

            if(tab!=indent){
                warning ++ ;
                warningWriter(tab,indent);
            }

            unput(yytext[yyleng-1]);
            BEGIN INITIAL;
}

<NEWLINE_STATE>((\t)*" "(\t)*)+. {
         //cout<<"rule 1 at line "<<line<<" global: "<<globalIndent<<" read: "<<yytext[yyleng-1]<<endl;
         warning ++ ;
         logFile<<"Line# "<<line<<": Warning, tab required but got space."<<endl;
         unput(yytext[yyleng-1]);
         BEGIN INITIAL;
}  


{WHITESPACE}    { }

if {lexLogWriter(toUpper(yytext),yytext); return IF; }
else {lexLogWriter(toUpper(yytext),yytext); return ELSE;}
for {lexLogWriter(toUpper(yytext),yytext);return FOR;}
while {lexLogWriter(toUpper(yytext),yytext);return WHILE;}
do {lexLogWriter(toUpper(yytext),yytext);return DO;}
break {lexLogWriter(toUpper(yytext),yytext);return BREAK;}
int {lexLogWriter(toUpper(yytext),yytext);return INT;}
char {lexLogWriter(toUpper(yytext),yytext);return CHAR;}
float {lexLogWriter(toUpper(yytext),yytext);return FLOAT;}
double {lexLogWriter(toUpper(yytext),yytext);return DOUBLE;}
void {lexLogWriter(toUpper(yytext),yytext);return VOID;}
return {lexLogWriter(toUpper(yytext),yytext);return RETURN;}
switch {lexLogWriter(toUpper(yytext),yytext);return SWITCH;}
case {lexLogWriter(toUpper(yytext),yytext);return CASE;}
default {lexLogWriter(toUpper(yytext),yytext);return DEFAULT;}
continue {lexLogWriter(toUpper(yytext),yytext);return CONTINUE;}
println {lexLogWriter(toUpper(yytext),yytext);return PRINTLN;}

{INT}      {
                lexLogWriter("CONST_INT",yytext);
                tokenFileWriter("CONST_INT",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"CONST_INT"); 
                return CONST_INT ;

            }


{FLOAT}    {
                lexLogWriter("CONST_FLOAT",yytext); 
                tokenFileWriter("CONST_FLOAT",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"CONST_FLOAT"); 
                return CONST_FLOAT ;
                
                }


{ID}        {
                lexLogWriter("ID",yytext) ;
                yylval.symbolInfo = new SymbolInfo(yytext,"ID");
                return ID ;
                //tokenFileWriter("ID",yytext);
                //insertSymbol("ID",yytext);

            }

{INVALID_ID_SUFFIX_NUM_PREFIX} {
               errorWriter("INVALID_ID_SUFFIX_NUM_PREFIX",yytext);  
                }
{TOO_MANY_DECIMAL_POINTS} {

            errorWriter("TOO_MANY_DECIMAL_POINTS",yytext);
}

{ILLFORMED_NUMBER} {
            errorWriter("ILLFORMED_NUMBER",yytext);
}


{ADDOP}     {
                lexLogWriter("ADDOP",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"ADDOP");
                return ADDOP ;
                //tokenFileWriter("ADDOP",yytext);
            }
{MULOP}     {
                lexLogWriter("MULOP",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"MULOP");
                return MULOP ;
                tokenFileWriter("MULOP",yytext);
            }
{INCOP}     {
                lexLogWriter("INCOP",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"INCOP");
                return INCOP ;
                tokenFileWriter("INCOP",yytext);
            }
{DECOP}     {
                lexLogWriter("DECOP",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"DECOP");
                return DECOP ;
                tokenFileWriter("DECOP",yytext);
            }
{RELOP}     {
                lexLogWriter("RELOP",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"RELOP");
                return RELOP ;
                tokenFileWriter("RELOP",yytext);
            }
{ASSIGNOP}  {
                lexLogWriter("ASSIGNOP",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"ASSIGNOP");
                return ASSIGNOP;
                tokenFileWriter("ASSIGNOP",yytext);
            }
{LOGICOP}   {
                lexLogWriter("LOGICOP",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"LOGICOP");
                return LOGICOP ;
                tokenFileWriter("LOGICOP",yytext);
            }
{BITOP}     {
                lexLogWriter("BITOP",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"BITOP");
                return BITOP;
                tokenFileWriter("BITOP",yytext);
            }
{NOT}       {
                lexLogWriter("NOT",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"NOT");
                return NOT;
                tokenFileWriter("NOT",yytext);
            }
{LPAREN}    {
                lexLogWriter("LPAREN",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"LPAREN");
                return LPAREN ;
                tokenFileWriter("LPAREN",yytext);
            }
{RPAREN}    {
                lexLogWriter("RPAREN",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"RPAREN");
                return RPAREN;
                tokenFileWriter("RPAREN",yytext);
            }
{LCURL}     {
                lexLogWriter("LCURL",yytext);
                tokenFileWriter("LCURL",yytext);
                globalIndent++ ;
                yylval.symbolInfo = new SymbolInfo(yytext,"LCURL");
                return LCURL;
            }
{RCURL}     {
                lexLogWriter("RCURL",yytext);
                tokenFileWriter("RCURL",yytext);
                if(globalIndent!=0)
                    globalIndent -- ;
                yylval.symbolInfo = new SymbolInfo(yytext,"RCURL");
                return RCURL;
            }
{LSQUARE}   {
                lexLogWriter("LSQUARE",yytext);
                tokenFileWriter("LSQUARE",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"LSQUARE");
                return LTHIRD;
            }
{RSQUARE}   {
                lexLogWriter("RSQUARE",yytext);
                tokenFileWriter("RSQUARE",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"RSQUARE");
                return RTHIRD;
            }
{COMMA}     {
                lexLogWriter("COMMA",yytext);
                tokenFileWriter("COMMA",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"COMMA");
                return COMMA ;
            }
{SEMICOLON} {
                lexLogWriter("SEMICOLON",yytext);
                tokenFileWriter("SEMICOLON",yytext);
                yylval.symbolInfo = new SymbolInfo(yytext,"SEMICOLON");
                return SEMICOLON;
            }


'  {
    lexeme += yytext ;
    characterLiteral = "" ;
   // cout<<"entering character state"<<endl;
    BEGIN CHAR_STATE ;
}

<CHAR_STATE>' {

    //cout<<"ending char_state"<<endl;

    lexeme += yytext;

    if (characterLiteral.empty()) {

        errorWriter("EMPTY_CONST_CHAR", lexeme);
        lexeme.clear();
        characterLiteral.clear();
        BEGIN INITIAL;

    } else if (characterLiteral.length() == 1 && characterLiteral[0] == '\\') {

        characterLiteral += yytext;

    } else if ((characterLiteral[0] == '\\' && characterLiteral.length() == 2) ||
               characterLiteral.length() == 1) {

        char c = extractChar(characterLiteral);

        if (c == -1) {

            errorWriter("UNKNOWN_ESCAPE_SEQUENCE", lexeme);

        } else {
            string token = "CONST_CHAR";
            characterLiteral = "";
            characterLiteral.push_back(c);
            tokenFileWriter(token, characterLiteral);
            lexLogWriter(token,characterLiteral);

        }

        lexeme.clear();
        characterLiteral.clear();
        BEGIN INITIAL;

    } else if (characterLiteral.length() > 1) {

        errorWriter("MULTICHAR_CONST_CHAR", lexeme);
        lexeme.clear();
        characterLiteral.clear();
        BEGIN INITIAL;
    }
}

<CHAR_STATE>[^\r\n\'] {
   //cout<<"matching character "<<yytext<<endl;
    lexeme += yytext ;
    characterLiteral += yytext ;
}

<CHAR_STATE>{NEWLINE} {
    errorWriter("UNFINISHED_CONST_CHAR", lexeme);
    line++;
    lexeme.clear();
    characterLiteral.clear();
    BEGIN INITIAL;
}

<CHAR_STATE><<EOF>> {
        errorWriter("UNFINISHED_CONST_CHAR", lexeme);
        lexeme.clear();
        characterLiteral.clear();
        endOfFile();
        //summaryWriter();
        return 0 ;
        }

\"      {   //cout<<"entering string_state"<<endl;
            tempLine = line ;
            characterLiteral = "";
            lexeme = yytext;
            isMultiLineString = false;
            BEGIN STRING_STATE;
        }


<STRING_STATE>\\[^ \r\n]  {
          
                lexeme += yytext ;
                char ch = extractChar(yytext);

                if(ch==-1){
                        characterLiteral += yytext;
                }else if(ch=='\b' && characterLiteral.length()>0){
                        characterLiteral.pop_back();
                }else{
                    characterLiteral.push_back(ch);
                }
                
                 }           

<STRING_STATE>\\{NEWLINE} {
                        line++ ;
                        lexeme += yytext;
                        isMultiLineString = true;
                    }

<STRING_STATE>\"   {    
                        string token ;

                        if(isMultiLineString){
                            token = "MULTI LINE STRING";
                        }else{
                            token = "SINGLE LINE STRING";
                        }
                        lexeme += yytext ;
                        tokenFileWriter(token,characterLiteral);
                        lexLogWriter(token,lexeme,tempLine);

                        characterLiteral.clear();
                        lexeme.clear();
                        BEGIN INITIAL ;

                   }

<STRING_STATE>{NEWLINE} {
                            errorWriter("UNFINISHED_STRING",lexeme);
                            line ++ ;
                            characterLiteral.clear();
                            lexeme.clear();
                            BEGIN INITIAL;
                        }

<STRING_STATE>[^\r\n\"] {
            characterLiteral += yytext ;
            lexeme += yytext ;
        }

<STRING_STATE><<EOF>>  {
                            
                            errorWriter("UNFINISHED_STRING",lexeme);
                            characterLiteral.clear();
                            lexeme.clear();
                            endOfFile();
                            //summaryWriter();
                            return 0 ;     
                        }
"//"[^\r\n]*  {
    lexLogWriter("SINGLE LINE COMMENT",yytext);

}

"//"[^\r\n]*\\{NEWLINE}    {
            tempLine = line ;
            lexeme += yytext ;
            line ++ ;
            //cout<<"entering single line comment state"<<endl;
            BEGIN SINGLE_LINE_COMMENT_STATE;
        }

<SINGLE_LINE_COMMENT_STATE>[^\n\r]*  { 

            lexeme += yytext ;
            // cout<<yytext<<endl;
        
}

<SINGLE_LINE_COMMENT_STATE>{NEWLINE} {
        lexLogWriter("SINGLE LINE COMMENT",lexeme,tempLine);
        line ++ ;
        tempLine=0;
        lexeme.clear();
        BEGIN INITIAL ;
}

<SINGLE_LINE_COMMENT_STATE><<EOF>> {
    lexLogWriter("SINGLE LINE COMMENT",lexeme,tempLine);
    lexeme.clear();
    tempLine=0;
    endOfFile();
   // summaryWriter();
    return 0 ;
}

"/*"  {
        tempLine = line;
        lexeme = yytext ;
        //cout<<"entering multi line comment state"<<endl;
        BEGIN MULTILINE_COMMENT_STATE;
}

<MULTILINE_COMMENT_STATE>"*/" {
    lexeme += yytext ;
    lexLogWriter("MULTI LINE COMMENT",lexeme,tempLine);
    lexeme.clear();
    tempLine=0;
    BEGIN INITIAL ;
}

<MULTILINE_COMMENT_STATE>[^\n\r] {
    lexeme += yytext ;
}

<MULTILINE_COMMENT_STATE>{NEWLINE} {
    line ++ ;
    lexeme += yytext ;
}

<MULTILINE_COMMENT_STATE><<EOF>> {
    errorWriter("UNFINISHED_COMMENT",lexeme);
    lexeme.clear();
    tempLine=0;
    endOfFile();
    //summaryWriter();
    return 0 ;
}

<<EOF>> {
            endOfFile();
            //summaryWriter();
            return 0;
}
            
.   {
        errorWriter("UNRECOGNIZED_CHAR",yytext);
    } 


%%

