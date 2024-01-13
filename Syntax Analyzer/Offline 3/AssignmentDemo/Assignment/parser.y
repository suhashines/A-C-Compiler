%{
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<fstream>
#include<cmath>
#include "2005037_SymbolTable.cpp"
#define YYSTYPE SymbolInfo*

using namespace std;

int yyparse(void);
int yylex(void);

extern int line ;
extern FILE *yyin;

//SymbolTable *table;

ofstream logFile ;
ofstream errorFile;

void yyerror(char *s)
{
	//write your code
	cout<<"Line: "<<line<<"# error "<<s<<endl;
}


void logFileWriter(const string &left,const string &right){
	logFile<<left<<" : "<<right<<endl;
}


%}

%token IF ELSE FOR WHILE DO BREAK INT CHAR FLOAT DOUBLE VOID RETURN SWITCH CASE DEFAULT CONTINUE PRINTLN
%token STRING NOT LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD BITOP COMMA SEMICOLON ASSIGNOP 
%token CONST_INT CONST_FLOAT CONST_CHAR LOGICOP RELOP ADDOP MULOP INCOP DECOP ID


%left COMMA
%right ASSIGNOP
%left LOGICOP
%left RELOP
%left ADDOP
%left MULOP
%left LCURL RCURL
%left LPAREN RPAREN

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%%

start : program
	{
		logFileWriter("start","program");
	}
	;

program : program unit {
	logFileWriter("program","program unit");
}
	| unit {logFileWriter("program","program unit");}
	;
	
unit : var_declaration {
	logFileWriter("unit","var_declaration");
}
     | func_declaration {
		logFileWriter("unit","func_declaration");
	 }
     | func_definition {
		logFileWriter("unit","func_definition");
	 }
     ;
     
func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON {
	logFileWriter("func_declaration","type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
}
		| type_specifier ID LPAREN RPAREN SEMICOLON {
			logFileWriter("func_declaration","type_specifier ID LPAREN RPAREN SEMICOLON");
		}
		;
		 
func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement {
	logFileWriter("func_definition","type_specifier ID LPAREN parameter_list RPAREN compound_statement");
}
		| type_specifier ID LPAREN RPAREN compound_statement {
			logFileWriter("func_definition","type_specifier ID LPAREN RPAREN compound_statement");
		}
 		;				


parameter_list  : parameter_list COMMA type_specifier ID {logFileWriter("parameter_list","parameter_list COMMA type_specifier ID");}
		| parameter_list COMMA type_specifier {logFileWriter("parameter_list","parameter_list COMMA type_specifier");}
 		| type_specifier ID {logFileWriter("parameter_list","type_specifier ID");}
		| type_specifier {logFileWriter("parameter_list","type_specifier");}
 		;

 		
compound_statement : LCURL statements RCURL {logFileWriter("compound_statement","LCURL statements RCURL");}
 		    | LCURL RCURL {logFileWriter("compound_statement","LCURL RCURL");}
 		    ;
 		    
var_declaration : type_specifier declaration_list SEMICOLON {logFileWriter("var_declaration","type_specifier declaration_list SEMICOLON");}
 		 ;
 		 
type_specifier	: INT  {
							logFileWriter("type_specifier","INT");
					   }
 		| FLOAT        {
							logFileWriter("type_specifier", "FLOAT");
					   }
 		| VOID		  {
							logFileWriter("type_specifier", "VOID");
		}
 		;
 		
declaration_list : declaration_list COMMA ID {
	logFileWriter("declaration_list","declaration_list COMMA ID");
}
 		  | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD {
			logFileWriter("declaration_list","declaration_list COMMA ID LTHIRD CONST_INT RTHIRD");
		  }
 		  | ID {
			logFileWriter("declaration_list","ID");
			}
 		  | ID LTHIRD CONST_INT RTHIRD  {
			logFileWriter("declaration_list","ID LTHIRD CONST_INT RTHIRD");
		  }
 		  ;
 		  
statements : statement {logFileWriter("statements","statement");}
	   | statements statement {
		logFileWriter("statements","statements statement");
	   }
	   ;
	   
statement : var_declaration {
	logFileWriter("statement","var_declaration");
}
	  | expression_statement {
		logFileWriter("statement","expression_statement");
	  }
	  | compound_statement {
		logFileWriter("statement","compound_statement");
	  }
	  | FOR LPAREN expression_statement expression_statement expression RPAREN statement {
		logFileWriter("statement","FOR LPAREN expression_statement expression_statement expression RPAREN statement");
	  }
	  | IF LPAREN expression RPAREN statement {
		logFileWriter("statement","IF LPAREN expression RPAREN statement");
	  }
	  | IF LPAREN expression RPAREN statement ELSE statement {
		logFileWriter("statement","IF LAPAREN expression RPAREN statement ELSE statement");
	  }
	  | WHILE LPAREN expression RPAREN statement {
		logFileWriter("statement","WHILE LPAREN expression RPAREN statement");
	  }
	  | PRINTLN LPAREN ID RPAREN SEMICOLON  {
		logFileWriter("statement","PRINTLN LPAREN ID RPAREN SEMICOLON");
	  }
	  | RETURN expression SEMICOLON {
		logFileWriter("statement","RETURN expression SEMICOLON");
	  }
	  ;
	  
expression_statement 	: SEMICOLON	{logFileWriter("expression_statement","SEMICOLON");}		
			| expression SEMICOLON {logFileWriter("expression_statement","expression SEMICOLON");}
			;
	  
variable : ID {logFileWriter("variable","ID");}
	 | ID LTHIRD expression RTHIRD {
		logFileWriter("variable","ID LTHIRD expression RTHIRD");
	 }
	 ;
	 
 expression : logic_expression	{logFileWriter("expression","logic_expression");}
	   | variable ASSIGNOP logic_expression {
		logFileWriter("expression","variable ASSIGNOP logic_expression");
	   }	
	   ;
			
logic_expression : rel_expression 	{
	logFileWriter("logic_expression","rel_expression");
}
		 | rel_expression LOGICOP rel_expression {
			logFileWriter("logic_expression","rel_expression LOGICOP rel_expression");
		 }	
		 ;
			
rel_expression	: simple_expression {
	logFileWriter("rel_expression","simple_expression");
}
		| simple_expression RELOP simple_expression	{
			logFileWriter("rel_expression","simple_expression RELOP simple_expression");
		}
		;
				
simple_expression : term {logFileWriter("simple_expression","term");}
		  | simple_expression ADDOP term {
			logFileWriter("simple_expression","simple_expression ADDOP term");
		  }
		  ;
					
term :	unary_expression {logFileWriter("term","unary_expression");}
     |  term MULOP unary_expression {logFileWriter("term","term MULOP unary_expression");}
     ;

unary_expression : ADDOP unary_expression {logFileWriter("unary_expression","ADDOP unary_expression");} 
		 | NOT unary_expression {logFileWriter("unary_expression","NOT unary_expression");}
		 | factor {logFileWriter("unary_expression","factor");}
		 ;
	
factor	: variable {logFileWriter("factor","variable");}
	| ID LPAREN argument_list RPAREN {logFileWriter("factor","ID LPAREN argument_list RPAREN");}
	| LPAREN expression RPAREN {logFileWriter("factor","LPAREN expression RPAREN");}
	| CONST_INT  {logFileWriter("factor","CONST_INT");} 
	| CONST_FLOAT {logFileWriter("factor","CONST_FLOAT");}
	| variable INCOP {logFileWriter("factor","variable INCOP");} 
	| variable DECOP {logFileWriter("factor","variable DECOP");}
	;
	
argument_list : arguments {logFileWriter("argument_list","arguments");}
			  |
			  ;
	
arguments : arguments COMMA logic_expression {
	logFileWriter("arguments","arguments COMMA logic_expression");
}
	      | logic_expression {
			logFileWriter("arguments","logic_expression");
		  }
	      ;
 

%%
int main(int argc,char *argv[])
{
	FILE *fp ;

	// fp = fopen("input.txt", "r");

    if((fp=fopen(argv[1],"r"))==NULL)
	{
		printf("Cannot Open Input File.\n");
		exit(1);
	}

	logFile.open("log.txt");
	errorFile.open("error.txt");
	

	yyin=fp;
	yyparse();
	

	logFile.close();
	errorFile.close();
	fclose(fp);
	
	return 0;
}

