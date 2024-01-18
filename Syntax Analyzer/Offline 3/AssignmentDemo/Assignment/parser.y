%{
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<list>
#include<algorithm>
#include<set>
#include<fstream>
#include<cmath>
#include "2005037_ParseTreeNode.cpp"

using namespace std;

int yyparse(void);
int yylex(void);

extern int line ;
extern int error ;
extern FILE *yyin;
extern string toUpper(const string &str);

SymbolTable symbolTable(11);

ofstream logFile ;
ofstream errorFile;
ofstream parseFile ;

//necessary global variables 
string varType ;  // stores recent variable type
string funcName,funcReturnType ;
list<pair<string,string>> parameters ; //Contains the parameter list <name, type> of the currently declared function
list<string> argList ;  //contains the argument list while invoking a function

bool definingFunction = false ;
//to know the state of the function

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

void errorFileWriter(const string &errorMsg,int lineCount){
  
   errorFile<<"Line# "<<lineCount<<": "<<errorMsg<<endl;
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

    parseFile<<root->name<<" :"<<root->nameList << " \t<Line : " <<root->startLine <<"-"<<root->endLine<< ">\n";

    for(ParseTreeNode* node:root->children){
        printParseTree(node,space+1);
    }


}

void printScopeTable(){
	cout<<symbolTable.printAll()<<endl;
	logFile<<symbolTable.printAll();
}


/* necessary functions for recognizing semantic errors */


void handleIdDeclaration(ParseTreeNode* node,int size){
	//extracting information 

	string lexeme = node->lexeme ;
	string token = node->token;
	string idType = toUpper(varType) ;
	int lineCount = node->startLine;

	//looking for error 

	if(idType=="void"){
		errorFile<<"Line# "<<lineCount<<": Variable or field '"<<lexeme<<"' declared as void\n";
		error ++ ;
		return ;
	}

	if(symbolTable.containsFunction(lexeme)){
		errorFile<<"Line# "<<lineCount<<": '"<<lexeme<<"' redeclared as different kind of symbol\n";
		error ++ ;
		return ;
	}

	IdInfo * idInfo = (IdInfo*) symbolTable.lookupCurrentScope(lexeme);

	if(idInfo!=nullptr){
		if(idInfo->idType==idType){
			errorFile<<"Line# "<<lineCount<<": Multiple declaration of '"<<lexeme<<"'\n";
			error ++ ;
		}else{
			errorFile<<"Line# "<<lineCount<<": Conflicting types for '"<<lexeme<<"'\n";
			error ++ ;
		}

		return ;
	}
    // no error in declaration, insert in symbolTable
	symbolTable.insert(lexeme,token,idType,size);
	cout<<lexeme<<" inserted in table "<<endl;
	cout<<symbolTable.printAll()<<endl;

}

void handleFunctionDeclaration(const string&name,const string&returnType,int lineCount){
	funcName = name ;
	funcReturnType = returnType ;
	FunctionInfo *functionInfo ;
    cout<<"function called\n";
	if(symbolTable.insert(name,"ID",true)){
	  //insertion successful
	  //let's print the symbolTable 
	  cout<<symbolTable.printAll();

	  functionInfo = (FunctionInfo*)symbolTable.lookupCurrentScope(name);
	  functionInfo->setReturnType(returnType);
	  functionInfo->isDefined = definingFunction;
	  //now we need to set its parameter list
	  set<string> paramName ;

      list <pair<string,string>>::iterator it = parameters.begin();

	  while(it!=parameters.end()){

		if(it->second!="" && !paramName.insert(it->second).second){
            string error = "Redefinition of parameter '"+it->second+"'";
			errorFileWriter(error,lineCount);
		}
		functionInfo->addParameter(it->first);
		it++ ;
	  }


	}else{
		SymbolInfo *symbol = symbolTable.lookup(name);

		if(!symbol->isFunction){
			string error = "'"+name+"' redeclared as different kind of symbol" ;
			errorFileWriter(error,lineCount);
		}else{
			//this function was declared or defined previously
			cout<<"this function was declared or defined previously\n";

			functionInfo = (FunctionInfo*)symbol ;
			cout<<"new funcs return type "<<returnType<<endl ;

			if(functionInfo->getReturnType()!=returnType){
				string error = "Conflicting types for '"+name+"'";
			    errorFileWriter(error,lineCount);
			}else if(!functionInfo->isDefined && definingFunction){
				//previous one is function prototype and this one is the definiton
                
				if(parameters.size() != functionInfo->getNumberOfParameters()){
					string error = "Conflicting types for '"+name+"'";
					errorFileWriter(error,lineCount);
				}else{
					//let's check for type mismatch

					int i=0 ;

					for(auto it=parameters.begin();it!=parameters.end();it++){
						
						if(it->first!=functionInfo->findParamAtIndex(i)){
							string error = "Type mismatch of argument "+to_string(i+1)+"of '"+name+"'";
							errorFileWriter(error,lineCount);
						}
						i++ ;
					}
				}

			}else{
				string error = "Multiple definition of function '"+name+"'" ;
				errorFileWriter(error,lineCount);
			}
			
		}
	}
    
	//resetting the variables 
	parameters.clear();
	definingFunction = false;

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
		printScopeTable();
	}
	;

program : program unit {
	logFileWriter("program","program unit");
	$$ = new ParseTreeNode("program");
	$$->addChild($1);
	$$->addChild($2);
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
		$$ = new ParseTreeNode("unit");
	    $$->addChild($1);
		definingFunction= false ;
	 }
     | func_definition {
		logFileWriter("unit","func_definition");
		$$ = new ParseTreeNode("unit");
	    $$->addChild($1);
		definingFunction= true;
	 }
     ;
     
func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON {
	logFileWriter("func_declaration","type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
    $$ = new ParseTreeNode("func_declaration");
	$$->addChild($1);
	$$->addChild($2);
	$$->addChild($3);
	$$->addChild($4);
	$$->addChild($5);
	$$->addChild($6);
    
	funcName = $2->lexeme ;
	int discoveryLine = $2->startLine;

	funcReturnType = $1->lastFoundLexeme ;
	handleFunctionDeclaration(funcName,toUpper(funcReturnType),discoveryLine);

}
		| type_specifier ID LPAREN RPAREN SEMICOLON {
			logFileWriter("func_declaration","type_specifier ID LPAREN RPAREN SEMICOLON");
		    $$ = new ParseTreeNode("func_declaration");
			$$->addChild($1);
	        $$->addChild($2);
	        $$->addChild($3);
	        $$->addChild($4);
	        $$->addChild($5);

			//at this point varType = returnType ;
	         funcName = $2->lexeme ;
	         int discoveryLine = $2->startLine;
			 funcReturnType = $1->lastFoundLexeme ;
	        handleFunctionDeclaration(funcName,toUpper(funcReturnType),discoveryLine);
		}
		;
		 
func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement {
	logFileWriter("func_definition","type_specifier ID LPAREN parameter_list RPAREN compound_statement");
   
    $$ = new ParseTreeNode("func_definition");
	 
	$$->addChild($1);
	//cout<<"child 1 inserted"<<endl;
	$$->addChild($2);
	//cout<<"Let's see child 2 "<<$2->lexeme<<endl;
	//cout<<"child 2 inserted"<<endl;
	$$->addChild($3);
	//cout<<"child 3 inserted"<<endl;
	$$->addChild($4);
	//cout<<"child 4 inserted"<<endl;
    $$->addChild($5);
	//cout<<"child 5 inserted"<<endl;
	$$->addChild($6);
	//cout<<"child 6 inserted"<<endl;
    
	funcName = $2->lexeme ;
	int discoveryLine = $2->startLine;
	funcReturnType = $1->lastFoundLexeme ;
	//cout<<"My return type is "<<funcReturnType<<endl;
	handleFunctionDeclaration(funcName,toUpper(funcReturnType),discoveryLine);

	
}
		| type_specifier ID LPAREN RPAREN compound_statement {
			logFileWriter("func_definition","type_specifier ID LPAREN RPAREN compound_statement");
		    $$ = new ParseTreeNode("func_definition");
			$$->addChild($1);
	        $$->addChild($2);
	        $$->addChild($3);
	        $$->addChild($4);
	        $$->addChild($5);
		
		    funcName = $2->lexeme ;
	        int discoveryLine = $2->startLine;
	        funcReturnType = $1->lastFoundLexeme ;
			//cout<<"My return type is "<<funcReturnType<<endl;
	        handleFunctionDeclaration(funcName,toUpper(funcReturnType),discoveryLine);

		}
 		;				


parameter_list  : parameter_list COMMA type_specifier ID {
	logFileWriter("parameter_list","parameter_list COMMA type_specifier ID");
	$$ = new ParseTreeNode("parameter_list");
	$$->addChild($1);
	$$->addChild($2);
	$$->addChild($3);
	$$->addChild($4);

    string paramName = $4->lexeme ;
	//now we'll insert varType and lexeme in our paramList
	parameters.push_back(make_pair(toUpper(varType),paramName));
    

	}
		| parameter_list COMMA type_specifier {
			logFileWriter("parameter_list","parameter_list COMMA type_specifier");
			$$ = new ParseTreeNode("parameter_list");
			$$->addChild($1);
		    $$->addChild($2);
		    $$->addChild($3);

			parameters.push_back(make_pair(toUpper(varType),""));

			}
 		| type_specifier ID {
			logFileWriter("parameter_list","type_specifier ID");
			$$ = new ParseTreeNode("parameter_list");
			$$->addChild($1);
		    $$->addChild($2);

			string paramName = $2->lexeme;

			parameters.push_back(make_pair(toUpper(varType),paramName));

			}
		| type_specifier {
			logFileWriter("parameter_list","type_specifier");
			$$ = new ParseTreeNode("parameter_list");
			$$->addChild($1);
			parameters.push_back(make_pair(toUpper(varType),""));
			}
 		;

 		
compound_statement : LCURL statements RCURL {
	logFileWriter("compound_statement","LCURL statements RCURL");
    $$ = new ParseTreeNode("compound_statement");
	$$->addChild($1);
	$$->addChild($2);
	$$->addChild($3);
	printScopeTable();
	symbolTable.exitScope();
}
 		    | LCURL RCURL {
				logFileWriter("compound_statement","LCURL RCURL");
			    $$ = new ParseTreeNode("compound_statement");
				$$->addChild($1);
				$$->addChild($2);
		        printScopeTable();
	            symbolTable.exitScope();
			}
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
	
	handleIdDeclaration($3,-1);

	$$ = new ParseTreeNode("declaration_list");

	$$->addChild($1);
	$$->addChild($2);
	$$->addChild($3);
	

}
 		  | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD {
			logFileWriter("declaration_list","declaration_list COMMA ID LSQUARE CONST_INT RSQUARE");
			$$ = new ParseTreeNode("declaration_list");
			$$->addChild($1);
		    $$->addChild($2);
		    $$->addChild($3);
		    $$->addChild($4);
		    $$->addChild($5);
		    $$->addChild($6);

			int size = stoi($5->lexeme) ;
			cout<<"found array size "<<size<<endl;
            handleIdDeclaration($3,size);
			
		  }
 		  | ID {
			logFileWriter("declaration_list","ID");
			handleIdDeclaration($1,-1); // -1 indicating that it's not an array

			$$ = new ParseTreeNode("declaration_list");
			$$->addChild($1);
			
			}
 		  | ID LTHIRD CONST_INT RTHIRD  {
			logFileWriter("declaration_list","ID LTHIRD CONST_INT RTHIRD");
			$$ = new ParseTreeNode("declaration_list");
			$$->addChild($1);
			$$->addChild($2);
			$$->addChild($3);
			$$->addChild($4);

			int size = stoi($3->lexeme) ;
			cout<<"found array size "<<size<<endl;
            handleIdDeclaration($1,size);
		  }
 		  ;
 		  
statements : statement {logFileWriter("statements","statement");
    $$ = new ParseTreeNode("statements");
	$$->addChild($1);
}
	   | statements statement {
		logFileWriter("statements","statements statement");
	
	    $$ = new ParseTreeNode("statements");
		$$->addChild($1);   
		$$->addChild($2);   
	   }
	   ;
	   
statement : var_declaration {
	logFileWriter("statement","var_declaration");
    $$ = new ParseTreeNode("statement");
    $$->addChild($1);
}
	  | expression_statement {
		logFileWriter("statement","expression_statement");
	    $$ = new ParseTreeNode("statement");
		$$->addChild($1);
	  }
	  | compound_statement {
		logFileWriter("statement","compound_statement");
	    $$ = new ParseTreeNode("statement");
		$$->addChild($1);
		symbolTable.exitScope();
	  }
	  | FOR LPAREN expression_statement expression_statement expression RPAREN statement {
		logFileWriter("statement","FOR LPAREN expression_statement expression_statement expression RPAREN statement");
	    $$ = new ParseTreeNode("statement");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);
		$$->addChild($6);
		$$->addChild($7);
	  }
	  | IF LPAREN expression RPAREN statement {
		logFileWriter("statement","IF LPAREN expression RPAREN statement");
	    $$ = new ParseTreeNode("statement");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);
	  }
	  | IF LPAREN expression RPAREN statement ELSE statement {
		logFileWriter("statement","IF LAPAREN expression RPAREN statement ELSE statement");
	    $$ = new ParseTreeNode("statement");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);
		$$->addChild($6);
		$$->addChild($7);
	  }
	  | WHILE LPAREN expression RPAREN statement {
		logFileWriter("statement","WHILE LPAREN expression RPAREN statement");
	    $$ = new ParseTreeNode("statement");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);
	  }
	  | PRINTLN LPAREN ID RPAREN SEMICOLON  {
		logFileWriter("statement","PRINTLN LPAREN ID RPAREN SEMICOLON");
	    $$ = new ParseTreeNode("statement");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);
	  }
	  | RETURN expression SEMICOLON {
		logFileWriter("statement","RETURN expression SEMICOLON");
	    $$ = new ParseTreeNode("statement");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
	  }
	  ;
	  
expression_statement : SEMICOLON	{
	logFileWriter("expression_statement","SEMICOLON");
    $$ = new ParseTreeNode("expression_statement");
	$$->addChild($1);

}		
			| expression SEMICOLON {
				logFileWriter("expression_statement","expression SEMICOLON");
			    $$ = new ParseTreeNode("expression_statement");
				$$->addChild($1);
				$$->addChild($2);
			
			}
			;
	  
variable : ID {logFileWriter("variable","ID");

$$ = new ParseTreeNode("variable");
$$->addChild($1);

//let's look for semantic errors

SymbolInfo *symbolCurr = symbolTable.lookupCurrentScope($1->lexeme);
SymbolInfo * symbolGlobal = symbolTable.lookupGlobalScope($1->lexeme);

if(symbolCurr ==nullptr && symbolGlobal ==nullptr){
    string error = "Undeclared variable '"+$1->lexeme+"'" ;
	errorFileWriter(error,$1->startLine);
}else{
	SymbolInfo *symbol = symbolCurr==nullptr ? symbolGlobal : symbolCurr ;
	// cout<<"let's see if I have this data type";
	// cout<<symbol->dataType<<endl ;
	$$ -> dataType = symbol->dataType ;
}

}
	 | ID LTHIRD expression RTHIRD {
		logFileWriter("variable","ID LTHIRD expression RTHIRD");
		
		$$ = new ParseTreeNode("variable");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		
		
SymbolInfo *symbolCurr = symbolTable.lookupCurrentScope($1->lexeme);
SymbolInfo * symbolGlobal = symbolTable.lookupGlobalScope($1->lexeme);

if(symbolCurr ==nullptr && symbolGlobal ==nullptr){
    string error = "Undeclared variable '"+$1->lexeme+"'" ;
	errorFileWriter(error,$1->startLine);
	
}else{

SymbolInfo *symbol = symbolCurr == nullptr?symbolGlobal : symbolCurr ;
string error = "'"+$1->lexeme+"' is not an array" ;

if(symbol->isFunction){
	
    errorFileWriter(error,$1->startLine);
}else{
	IdInfo* idInfo = (IdInfo*)symbol ;
	//cout<<"symbol is an id"<<endl;
	if(idInfo->size==-1){
		errorFileWriter(error,$1->startLine);
		//cout<<"symbol is not an array"<<endl;
	}else if($3->lastFoundToken=="CONST_FLOAT"){
		//cout<<"symbol is an array with invalid subscript"<<endl;
		errorFileWriter("Array subscript is not an integer",$1->startLine);
     }else{
		// I have found no error in array declaration 
		$$ -> dataType = idInfo->dataType ;
		//cout<<"yoo found correct array data type "<<$$->dataType<<endl;
	 }
}


}

		}
	 ;
	 
 expression : logic_expression	{logFileWriter("expression","logic_expression");
 $$ = new ParseTreeNode("expression");
 $$->addChild($1);
 
 }
	   | variable ASSIGNOP logic_expression {
		logFileWriter("expression","variable ASSIGNOP logic_expression");
		$$ = new ParseTreeNode("expression");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);


		
		}	
	   ;
			
logic_expression : rel_expression 	{
	logFileWriter("logic_expression","rel_expression");
    $$ = new ParseTreeNode("logic_expression");
	$$->addChild($1);
	$$->dataType = $1->dataType ;

}
		 | rel_expression LOGICOP rel_expression {
			logFileWriter("logic_expression","rel_expression LOGICOP rel_expression");
		     $$ = new ParseTreeNode("logic_expression");
			 $$->addChild($1);
			 $$->addChild($2);
			 $$->addChild($3);
		 
		 }	
		 ;
			
rel_expression	: simple_expression {
	logFileWriter("rel_expression","simple_expression");
    $$=new ParseTreeNode("rel_expression");
	$$->addChild($1);
	$$->dataType = $1->dataType ;
}
		| simple_expression RELOP simple_expression	{
			logFileWriter("rel_expression","simple_expression RELOP simple_expression");
		    $$=new ParseTreeNode("rel_expression");
			$$->addChild($1);
			$$->addChild($2);
			$$->addChild($3);
		}
		;
				
simple_expression : term {logFileWriter("simple_expression","term");
$$ = new ParseTreeNode("simple_expression");
$$->addChild($1);
$$->dataType = $1->dataType ;

}
		  | simple_expression ADDOP term {
			logFileWriter("simple_expression","simple_expression ADDOP term");
			$$ = new ParseTreeNode("simple_expression");
			$$->addChild($1);
			$$->addChild($2);
			$$->addChild($3);

			}
		  ;
					
term :	unary_expression {
	logFileWriter("term","unary_expression");
	$$ = new ParseTreeNode("term");
	$$->addChild($1);
	$$->dataType = $1->dataType ;
	
	}
     |  term MULOP unary_expression {
		logFileWriter("term","term MULOP unary_expression");
		$$ = new ParseTreeNode("term");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		
		}
     ;

unary_expression : ADDOP unary_expression {
	logFileWriter("unary_expression","ADDOP unary_expression");
	$$ = new ParseTreeNode("unary_expression");
	$$->addChild($1);
	$$->addChild($2);
	} 
		 | NOT unary_expression {
			logFileWriter("unary_expression","NOT unary_expression");
			$$ = new ParseTreeNode("unary_expression");
			$$->addChild($1);
			$$->addChild($2);
			}
		 | factor {
			logFileWriter("unary_expression","factor");
			$$ = new ParseTreeNode("unary_expression");
			$$->addChild($1);
            $$->dataType = $1->dataType ;
			}
		 ;
	
factor	: variable {
	logFileWriter("factor","variable");
	$$ = new ParseTreeNode("factor");
	$$->addChild($1);
	$$->dataType = $1->dataType ;
	
	}
	| ID LPAREN argument_list RPAREN {
		logFileWriter("factor","ID LPAREN argument_list RPAREN");
		$$ = new ParseTreeNode("factor");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);

		SymbolInfo* symbol = symbolTable.lookup($1->lexeme);

		if(symbol == nullptr || (!symbol->isFunction) ){
			string error = "Undeclared function '"+$1->lexeme+"'";
			errorFileWriter(error,$1->startLine);
		}else{
			//our symbol is a function 
			FunctionInfo * func = (FunctionInfo*)symbol ;
			//but what are its arguments!!! already stored in argList ^_^

			cout<<"my size"<<argList.size()<<endl;

		}
		
		}
	| LPAREN expression RPAREN {
		logFileWriter("factor","LPAREN expression RPAREN");
		$$ = new ParseTreeNode("factor");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);

		}
	| CONST_INT  {
		logFileWriter("factor","CONST_INT");
		$$ = new ParseTreeNode("factor");
		$$->addChild($1);
		$$->dataType = "INT";
		
		} 
	| CONST_FLOAT {
		logFileWriter("factor","CONST_FLOAT");
		$$ = new ParseTreeNode("factor");
		$$->addChild($1);
		$$->dataType = "FLOAT";
		}
	| variable INCOP {
		logFileWriter("factor","variable INCOP");
		$$ = new ParseTreeNode("factor");
		$$->addChild($1);
		$$->addChild($2);
		$$->dataType = $1->dataType ;
		} 
	| variable DECOP {
		logFileWriter("factor","variable DECOP");
		$$ = new ParseTreeNode("factor");
		$$->addChild($1);
		$$->addChild($2);
		$$->dataType = $1->dataType ;
		}
	;
	
argument_list : arguments {
	logFileWriter("argument_list","arguments");
	$$ = new ParseTreeNode("argument_list");
	$$->addChild($1);

	}
			  |
			  ;
	
arguments : arguments COMMA logic_expression {
	logFileWriter("arguments","arguments COMMA logic_expression");
	$$ = new ParseTreeNode("arguments");
	$$->addChild($1);
	$$->addChild($2);
	$$->addChild($3);
	argList.push_back($3->dataType);
}
	      | logic_expression {
			logFileWriter("arguments","logic_expression");
			$$ = new ParseTreeNode("arguments");
			$$->addChild($1);
			//declare an argument list and push the arguments here
			argList.push_back($1->dataType);
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

