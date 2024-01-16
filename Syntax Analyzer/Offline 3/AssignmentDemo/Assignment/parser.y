%{
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<fstream>
#include<cmath>
#include "2005037_SymbolTable.cpp"
#include "2005037_ParseTreeNode.cpp"

using namespace std;

int yyparse(void);
int yylex(void);

extern int line ;
extern int error ;
extern FILE *yyin;

SymbolTable symbolTable(11);

ofstream logFile ;
ofstream errorFile;
ofstream parseFile ;

//necessary global variables 
string varType ;

void yyerror(char *s)
{
	//write your code
	cout<<"Line: "<<line<<"# error "<<s<<endl;
}


void printTable(){
	logFile<<symbolTable.printAll();
}


void logFileWriter(const string &left,const string &right){
	logFile<<left<<" : "<<right<<endl;
}

void summaryWriter(){
    logFile<<"Total Lines: "<<line<<endl;
    logFile<<"Total Errors: "<<error<<endl;
}

//here comes the beast who prints the entire parseTree

void printParseTree(ParseTreeNode *root, int space)
{

    for (int i = 0; i < space; i++)
    {
        parseFile << " ";
    }

    if (root->isLeaf)
    {
        parseFile << root->token << " : " << root->lexeme << "\t<Line : " << root->startLine << ">\n";
        return ;
    }

    parseFile<<root->name<<" :"<<root->nameList<< "\t<Line : " <<root->startLine <<"-"<<root->endLine<< ">\n";

    for(ParseTreeNode* node:root->children){
        printParseTree(node,space+1);
    }


}


/* necessary functions for recognizing semantic errors */


void handleIdDeclaration(ParseTreeNode* node){
	//extracting information 

	string lexeme = node->lexeme ;
	string token = node->token;
	string idType = varType ;
	int lineCount = node->startLine;

	//looking for error 

	if(idType=="void"){
		errorFile<<"Line# "<<lineCount<<": Variable or field '"<<lexeme<<"' declared as void\n";
		return ;
	}

	if(symbolTable.containsFunction(lexeme)){
		errorFile<<"Line# "<<lineCount<<": '"<<lexeme<<"' redeclared as different kind of symbol\n";
		return ;
	}

	IdInfo * idInfo = (IdInfo*) symbolTable.lookupCurrentScope(lexeme);

	if(idInfo!=nullptr){
		if(idInfo->idType==idType){
			errorFile<<"Line# "<<lineCount<<": '"<<"Multiple declaration of '"<<lexeme<<"'\n";
		}else{
			errorFile<<"Line# "<<lineCount<<": '"<<"Conflicting types for '"<<lexeme<<"'\n";
		}

		return ;
	}

	symbolTable.insert(lexeme,token,idType);

}








//////  grammar section ///////////////////////


%}

%union{
	ParseTreeNode* parseTreeNode;
}

%token <parseTreeNode> IF ELSE FOR WHILE DO BREAK INT CHAR FLOAT DOUBLE VOID RETURN SWITCH CASE DEFAULT CONTINUE PRINTLN
%token <parseTreeNode> NOT LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON INCOP DECOP ASSIGNOP 
%token <parseTreeNode> CONST_INT CONST_FLOAT LOGICOP RELOP ADDOP BITOP MULOP ID

%type <parseTreeNode> start program unit var_declaration func_declaration func_definition type_specifier parameter_list compound_statement statements declaration_list statement expression_statement expression variable logic_expression rel_expression simple_expression term unary_expression factor argument_list arguments


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
		summaryWriter();
		$$ = new ParseTreeNode("start");
		$$->addChild($1);
		printParseTree($$,0);
	}
	;

program : program unit {
	logFileWriter("program","program unit");
}
	| unit {logFileWriter("program","unit");
	
	$$ = new ParseTreeNode("program");
	$$->addChild($1);
	
	}
	;
	
unit : var_declaration {
	logFileWriter("unit","var_declaration");
	$$ = new ParseTreeNode("unit");
	$$->addChild($1);
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
 		    
var_declaration : type_specifier declaration_list SEMICOLON {logFileWriter("var_declaration","type_specifier declaration_list SEMICOLON");

$$ = new ParseTreeNode("var_declaration");
$$->addChild($1);
$$->addChild($2);
$$->addChild($3);

}
 		 ;
 		 
type_specifier	: INT  {
							logFileWriter("type_specifier","INT");
							varType = "int" ;
							$$ = new ParseTreeNode("type_specifier");
							$$->addChild($1);


					   }
 		| FLOAT        {
							logFileWriter("type_specifier", "FLOAT");
							varType = "float" ;
							$$ = new ParseTreeNode("type_specifier");
							$$->addChild($1);
					   }
 		| VOID		  {
							logFileWriter("type_specifier", "VOID");
							varType = "void" ;
							$$ = new ParseTreeNode("type_specifier");
							$$->addChild($1);
		}
 		;
 		
declaration_list : declaration_list COMMA ID {
	logFileWriter("declaration_list","declaration_list COMMA ID");
	
	handleIdDeclaration($3);

	$$ = new ParseTreeNode("declaration_list");

	$$->addChild($1);
	$$->addChild($2);
	$$->addChild($3);
	

}
 		  | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD {
			logFileWriter("declaration_list","declaration_list COMMA ID LSQUARE CONST_INT RSQUARE");
		  }
 		  | ID {
			logFileWriter("declaration_list","ID");
			handleIdDeclaration($1);

			$$ = new ParseTreeNode("declaration_list");
			$$->addChild($1);
			
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
	parseFile.open("parsetree.txt");
	

	yyin=fp;
	yyparse();
	

	logFile.close();
	errorFile.close();
	fclose(fp);
	
	return 0;
}

