/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 2
#define YYMINOR 0
#define YYPATCH 20220114

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#undef YYBTYACC
#define YYBTYACC 0
#define YYDEBUGSTR YYPREFIX "debug"
#define YYPREFIX "yy"

#define YYPURE 0

#line 2 "2005037.y"
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<list>
#include<algorithm>
#include<set>
#include<fstream>
#include<cmath>
#include "2005037_ParseTreeNode.cpp"
#include "2005037_Register.cpp"

int ParseTreeNode::labelCount = 0 ;

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
ofstream assemblyFile ;

/*necessary global variables */
string varType ;  /* stores recent variable type*/
string funcName,funcReturnType ;
list<pair<string,string>> parameters ; /*Contains the parameter list <name, type> of the currently declared function*/
list<string> argList ;  /*contains the argument list while invoking a function*/

bool definingFunction = false ;
/*to know the state of the function*/
bool isError = false ; /*to know if there has been syntax error in function call*/
bool hasDeclListError = false ;

/* ICG starts here  // */

int offset = 0; /*to track location of local variables into the stack*/
bool hasPrint = false;
Register reg ;   /* works as a register manager*/


void yyerror(char *s)
{
	/*write your code*/
	cout<<"Line: "<<line<<"# error "<<s<<endl;
}


/****************************************************************/


void genStartingCode(){

string str = 
	
".MODEL SMALL\n\
.STACK 1000H\n\
.Data\n\
\tnumber DB \"00000$\"\n" ;

assemblyFile<<str ;

/* let's gen data segment*/

string ds = "" ;

for(const string & vars: symbolTable.globalVariables){
	ds += "\t"+vars + " DW " ;

	IdInfo * id = (IdInfo*)symbolTable.lookup(vars) ;

	if(id->size == -1){
		ds += "1" ;
	}else{
		ds += to_string(id->size) ;
	}

	ds+= " DUP (0000H)\n" ;
}

assemblyFile<<ds ;

assemblyFile<<".CODE\n" ;

}


string getIdAdddress(const string & id){
	SymbolInfo* info = symbolTable.lookup(id);

	if(info->isGlobal){
		return info->getName();
	}

	return "[BP-"+to_string(info->offset)+"]";
}

void writeLabel(int label){
	assemblyFile<<"L"<<label<<":\n";
}

string formatLabel(int label){
	return "L"+to_string(label);
}

void traverseAndGenerate(ParseTreeNode*root){

	if(root->isLeaf){
		/*decision about leaf nodes have already been taken*/
		return ;
	}

	string rule = root->name+" :"+root->nameList ;

	if(rule=="rel_expression : simple_expression RELOP simple_expression"){
		
		traverseAndGenerate(root->children[0]);

		string l = reg.getRegister();
		string code = "\tMOV "+l+","+root->children[0]->addr+"\n" ;
		reg.resetRegister(root->children[0]->addr);
		assemblyFile<<code ;

		traverseAndGenerate(root->children[2]);

		string r = reg.getRegister();
		code = "\tMOV "+r+","+root->children[2]->addr+"\n" ;
		reg.resetRegister(root->children[2]->addr);

		string operation ;

		string lexeme = root->children[1]->lexeme ;

		if(lexeme=="<") operation = "JL" ;
		else if(lexeme=="<=") operation = "JLE" ;
		else if(lexeme==">") operation = "JG" ;
		else if(lexeme==">=") operation = "JGE" ;
		else if(lexeme=="==") operation = "JE" ;
		else if(lexeme=="!=") operation = "JNE" ;

		cout<<"operation selected "<<operation<<endl;

		
		code += "\tCMP "+l+","+r+"\n" ;

		root->label = (ParseTreeNode::getLabel()) ;

		code +=  "\t"+operation + " "+formatLabel(root->label) + "\n";

		int falseLabel = ParseTreeNode::getLabel() ;

		code += "\tj "+formatLabel(falseLabel) + "\n";

		assemblyFile<<code ;

		writeLabel(root->label);
		string dest = reg.getRegister();

		code = "\tMOV "+dest+",1\n";

		int finalLabel = ParseTreeNode::getLabel();

		code+= "\tj "+formatLabel(finalLabel) + "\n";

		assemblyFile<<code ;

		writeLabel(falseLabel);

		assemblyFile<<"\tMOV "+dest+",0\n" ;

		root->addr = dest ;

		root->label = finalLabel ;

		writeLabel(finalLabel);

		reg.resetRegister(l,r);

     	return ;

	}

	if(rule=="term : term MULOP unary_expression"){
		traverseAndGenerate(root->children[0]);
		traverseAndGenerate(root->children[2]);

		string multiplicand = root->children[0]->addr;
		string multiplier = root->children[2]->addr;
		string code ;
		if(multiplicand != "AX"){
			code += "\tMOV AX,"+multiplicand+"\n";
			reg.resetRegister(multiplicand);
			reg.acquireRegister("AX");
		}

		string operation ;

		if(root->children[1]->lexeme=="*"){
			operation= "MUL" ;
		}else{
			operation = "DIV" ;
		}


		code += "\t"+operation+" "+multiplier+"\n" ;

		reg.resetRegister(multiplier);

		assemblyFile<<code ;

		if(root->children[1]->lexeme=="%"){
			root->addr = "DX" ;
			reg.acquireRegister("DX");
			}
		else
			root->addr = "AX" ;

		return ;
	}

	if(rule=="simple_expression : simple_expression ADDOP term"){

		traverseAndGenerate(root->children[0]);

		/*cout<<"hola hola"<<root->children[0]->addr<<endl ;*/

		string dest = reg.getRegister();

		/*cout<<dest<<endl;*/
		
	    string code = "\tMOV "+dest+","+root->children[0]->addr+"\n" ;
		reg.resetRegister(root->children[0]->addr);
		assemblyFile<<code ;
		code = "";
		
		traverseAndGenerate(root->children[2]);

		string src = root->children[2]->addr;
		

		code += "\tADD "+dest+","+src+"\n";
		reg.resetRegister(src);
		root->addr = dest ; 

		assemblyFile<<code ;

		return ;
	}

	if(rule=="statement : PRINTLN LPAREN ID RPAREN SEMICOLON"){
		root->label = ParseTreeNode::getLabel();

		writeLabel(root->label);

		hasPrint = true;
		string code = "\tMOV AX,"+getIdAdddress(root->children[2]->lexeme)+"\n";
		code += "\tCALL print_output\n";
		code += "\tCALL new_line\n";

		assemblyFile<<code ;

		return ;
	}

	if(rule=="variable : ID"){
		root->addr = getIdAdddress(root->children[0]->lexeme);
		return ;
	}

	if(rule=="factor : CONST_INT"){

		root->addr = reg.getRegister() ;
		cout<<"factor register"<<root->addr<<endl;
		string code = "\tMOV "+root->addr+","+root->children[0]->lexeme+"\n";
		assemblyFile<<code;
		return ;
	}

	if(rule=="unary_expression : factor" || rule=="term : unary_expression" || rule=="simple_expression : term"||rule=="rel_expression : simple_expression"||rule=="logic_expression : rel_expression" || rule=="factor : variable"){
		traverseAndGenerate(root->children[0]);
		root->addr = root->children[0]->addr ;
		root->label = root->children[0]->label;
		return;
	}

	if(rule=="expression : variable ASSIGNOP logic_expression"){

		root->label = ParseTreeNode::getLabel();
		writeLabel(root->label);

		traverseAndGenerate(root->children[0]);
		traverseAndGenerate(root->children[2]);



		string src = root->children[2]->addr ;
		string dest = root->children[0]->addr ;

		string code="\tMOV "+dest+","+src+"\n" ;

		assemblyFile<<code;

		root->addr = dest ;

		/*now we can reset the source,dest is the new source*/
		reg.resetRegister(src);

		return ;
	}



	if(rule=="expression_statement : expression SEMICOLON" || rule=="statement : expression_statement" || rule=="statements : statement" || rule=="logic_expression : rel_expression"){
		traverseAndGenerate(root->children[0]);
		root->addr = root->children[0]->addr ;
		return ;
	}

	if(rule=="var_declaration : type_specifier declaration_list SEMICOLON"){
		
		if(!symbolTable.isCurrentScopeGlobal())
		 {    root->label = ParseTreeNode::getLabel() ;
		 	  writeLabel(root->label);
		 }

		traverseAndGenerate(root->children[1]);	

		return ;
	}

	if(rule=="compound_statement : LCURL statements RCURL"){
		
		symbolTable.enterScope();

		traverseAndGenerate(root->children[1]);	

		symbolTable.exitScope();

		/*reset the register*/
		root->addr = root->children[1]->addr ;

		return ;
	}

	if(rule=="declaration_list : ID"){
		offset += 2 ;
		string lexeme = root->children[0]->lexeme ;
		string token = root->children[0]->token ;

		symbolTable.insert(lexeme,token,"INT",-1,symbolTable.isCurrentScopeGlobal(),offset);

		if(!symbolTable.isCurrentScopeGlobal()){
			assemblyFile<<"\tSUB SP,2\n" ;
		}

		return ;
	}

	if(rule=="declaration_list : declaration_list COMMA ID"){

		traverseAndGenerate(root->children[0]) ;

		offset += 2 ;
		string lexeme = root->children[2]->lexeme ;
		string token = root->children[2]->token ;

		symbolTable.insert(lexeme,token,"INT",-1,symbolTable.isCurrentScopeGlobal(),offset);

		if(!symbolTable.isCurrentScopeGlobal()){
			assemblyFile<<"\tSUB SP,2\n" ;
		}

		return ;
	}



	if(rule=="func_definition : type_specifier ID LPAREN RPAREN compound_statement"){
		cout<<"func def rule matched"<<endl;
		offset = 0 ;
		string funcName = root->children[1]->lexeme ;

		string code = funcName+" PROC"+"\n" ;

			if(funcName == "main"){
				cout<<"symbol is main function\n";
				code += "\tMOV AX,@DATA\n" ;
				code += "\tMOV DS,AX\n" ;
			}

			code += "\tPUSH BP\n" ;
			code += "\tMOV BP,SP\n" ;

		assemblyFile<<code ;

		traverseAndGenerate(root->children[4]);

		assemblyFile<<"RETURN:\n" ;

		/*storing the return address*/
		SymbolInfo* symbol = symbolTable.lookup(root->children[1]->lexeme);
		symbol->returnAddr = root->children[4]->addr ;

		/*putting SP add amount here//*/
		assemblyFile<<"\tADD SP,"<<offset<<"\n" ;
		assemblyFile<<"\tPOP BP\n";
		if(root->children[1]->lexeme=="main"){
			/*cout<<"main func ended"<<endl;*/
			assemblyFile<<"\tMOV AX,4CH\n";
			assemblyFile<<"\tINT 21H\n" ;
			
		}else{
			assemblyFile<<"\tRET\n" ;
		}

		assemblyFile<<root->children[1]->lexeme<<" ENDP\n";

		return ;

	}


	for(ParseTreeNode*node : root->children){
		traverseAndGenerate(node);
	}


}

void genEndingCode(){

	if(hasPrint){
		/* code for including print function*/
		assemblyFile<<"\n\n;------print library-------;\n\n" ;
string code =
"new_line proc\n\
    push ax\n\
    push dx\n\
    mov ah,2\n\
    mov dl,0Dh\n\
    int 21h\n\
    mov ah,2\n\
    mov dl,0Ah\n\
    int 21h\n\
    pop dx\n\
    pop ax\n\
    ret\n\
    new_line endp\n\
print_output proc  ;print what is in ax\n\
    push ax\n\
    push bx\n\
    push cx\n\
    push dx\n\
    push si\n\
    lea si,number\n\
    mov bx,10\n\
    add si,4\n\
    cmp ax,0\n\
    jnge negate\n\
    print:\n\
    xor dx,dx\n\
    div bx\n\
    mov [si],dl\n\
    add [si],'0'\n\
    dec si\n\
    cmp ax,0\n\
    jne print\n\
    inc si\n\
    lea dx,si\n\
    mov ah,9\n\
    int 21h\n\
    pop si\n\
    pop dx\n\
    pop cx\n\
    pop bx\n\
    pop ax\n\
    ret\n\
    negate:\n\
    push ax\n\
    mov ah,2\n\
    mov dl,'-'\n\
    int 21h\n\
    pop ax\n\
    neg ax\n\
    jmp print\n\
    print_output endp\n" ;

	assemblyFile<<code ;

	}

	assemblyFile<<"END main\n" ;
}

void codeGenerator(ParseTreeNode*root){

     genStartingCode();

	 offset = 0 ;

	 traverseAndGenerate(root);

	 genEndingCode();

}


/* --------------------------------*/


void printTable(){
	logFile<<symbolTable.printAll();
}


void logFileWriter(const string &left,const string &right){
	logFile<<left<<" : "<<right<<" "<<endl;
}

void errorFileWriter(const string &errorMsg,int lineCount){
   error ++ ;
   errorFile<<"Line# "<<lineCount<<": "<<errorMsg<<endl;
}

void summaryWriter(){
    logFile<<"Total Lines: "<<line<<endl;
    logFile<<"Total Errors: "<<error<<endl;
}

/*here comes the beast who prints the entire parseTree*/

void printParseTree(ParseTreeNode *root, int space)
{

    for (int i = 0; i < space; i++)
    {
        parseFile << " ";
    }

    if (root->isLeaf)
    {
        parseFile << root->token << " : " << root->lexeme << "\t<Line: " << root->startLine << ">\n";
        return ;
    }

    parseFile<<root->name<<" :"<<root->nameList << " \t<Line: " <<root->startLine <<"-"<<root->endLine<< ">\n";

    for(ParseTreeNode* node:root->children){
        printParseTree(node,space+1);
    }


}

void printScopeTable(){
	/*cout<<symbolTable.printAll()<<endl;*/
	logFile<<symbolTable.printAll();
}


/* necessary functions for recognizing semantic errors */


void handleIdDeclaration(ParseTreeNode* node,int size){

	if(hasDeclListError){
		hasDeclListError = false;
		return ;
	}

	/*extracting information */

	string lexeme = node->lexeme ;
	string token = node->token;
	string idType = toUpper(varType) ;
	int lineCount = node->startLine;

	/*looking for error */

	if(idType=="VOID"){
		errorFile<<"Line# "<<lineCount<<": Variable or field '"<<lexeme<<"' declared void\n";
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
			errorFile<<"Line# "<<lineCount<<": Conflicting types for'"<<lexeme<<"'\n";
			error ++ ;
		}

		return ;
	}

	
	 offset += 2; 
    /* no error in declaration, insert in symbolTable*/
	symbolTable.insert(lexeme,token,idType,size,symbolTable.isCurrentScopeGlobal(),offset);
	/*cout<<lexeme<<" inserted in table "<<endl;*/
	/*cout<<symbolTable.printAll()<<endl;*/

}

void handleFunctionDeclaration(const string&name,const string&returnType,int lineCount){
	funcName = name ;
	funcReturnType = returnType ;
	FunctionInfo *functionInfo ;
    /*cout<<"function called\n";*/
	if(symbolTable.insert(name,"ID",true)){
	  /*insertion successful*/
	  /*let's print the symbolTable */
	  /*cout<<symbolTable.printAll();*/

	  functionInfo = (FunctionInfo*)symbolTable.lookupCurrentScope(name);
	  functionInfo->setReturnType(returnType);
	  functionInfo->isDefined = definingFunction;
	  /*now we need to set its parameter list*/
	  set<string> paramName ;

      list <pair<string,string>>::iterator it = parameters.begin();

	  while(it!=parameters.end()){

		if(it->second!="" && !paramName.insert(it->second).second){
            string error = "Redefinition of parameter '"+it->second+"'";
			errorFileWriter(error,lineCount);
		}else{
			functionInfo->addParameter(it->first);
		}
		
		it++ ;
	  }


	}else{
		SymbolInfo *symbol = symbolTable.lookup(name);

		if(!symbol->isFunction){
			string error = "'"+name+"' redeclared as different kind of symbol" ;
			errorFileWriter(error,lineCount);
		}else{
			/*this function was declared or defined previously*/
			/*cout<<"this function was declared or defined previously\n";*/

			functionInfo = (FunctionInfo*)symbol ;
			/*cout<<"new funcs return type "<<returnType<<endl ;*/

			if(functionInfo->getReturnType()!=returnType){
				string error = "Conflicting types for '"+name+"'";
			    errorFileWriter(error,lineCount);
			}else if(!functionInfo->isDefined && definingFunction){
				/*previous one is function prototype and this one is the definiton*/
                
				if(parameters.size() != functionInfo->getNumberOfParameters()){
					string error = "Conflicting types for '"+name+"'";
					errorFileWriter(error,lineCount);
				}else{
					/*let's check for type mismatch*/

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
    
	/*resetting the variables */
	parameters.clear();
	definingFunction = false;

}




/*////  grammar section ///////////////////////*/


#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 713 "2005037.y"
typedef union YYSTYPE{
	ParseTreeNode* parseTreeNode;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 745 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

#if !(defined(yylex) || defined(YYSTATE))
int YYLEX_DECL();
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define IF 257
#define ELSE 258
#define FOR 259
#define WHILE 260
#define DO 261
#define BREAK 262
#define INT 263
#define CHAR 264
#define FLOAT 265
#define DOUBLE 266
#define VOID 267
#define RETURN 268
#define SWITCH 269
#define CASE 270
#define DEFAULT 271
#define CONTINUE 272
#define PRINTLN 273
#define NOT 274
#define LPAREN 275
#define RPAREN 276
#define LCURL 277
#define RCURL 278
#define LTHIRD 279
#define RTHIRD 280
#define COMMA 281
#define SEMICOLON 282
#define COLON 283
#define INCOP 284
#define DECOP 285
#define ASSIGNOP 286
#define CONST_INT 287
#define CONST_FLOAT 288
#define LOGICOP 289
#define RELOP 290
#define ADDOP 291
#define BITOP 292
#define MULOP 293
#define ID 294
#define LOWER_THAN_ELSE 295
#define YYERRCODE 256
typedef int YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    1,    1,    2,    2,    2,    4,    4,    5,    5,
    7,    7,    7,    7,    7,    8,    8,    3,    6,    6,
    6,   10,   10,   10,   10,   10,    9,    9,   11,   11,
   11,   11,   11,   11,   11,   11,   11,   11,   23,   24,
   24,   25,   25,   26,   12,   12,   14,   14,   13,   13,
   13,   15,   15,   16,   16,   17,   17,   18,   18,   19,
   19,   19,   20,   20,   20,   20,   20,   20,   20,   21,
   21,   22,   22,
};
static const YYINT yylen[] = {                            2,
    1,    2,    1,    1,    1,    1,    6,    5,    6,    5,
    4,    3,    2,    1,    1,    3,    2,    3,    1,    1,
    1,    3,    6,    1,    4,    1,    1,    2,    1,    1,
    1,    7,    5,    7,    5,    5,    3,    1,    7,    2,
    1,    7,    6,    5,    1,    2,    1,    4,    1,    3,
    1,    1,    3,    1,    3,    1,    3,    1,    3,    2,
    2,    1,    1,    4,    3,    1,    1,    2,    2,    1,
    0,    3,    1,
};
static const YYINT yydefred[] = {                         0,
   19,   20,   21,    0,    0,    3,    4,    5,    6,    0,
    2,   26,    0,    0,    0,    0,    0,   18,   15,    0,
    0,    0,    0,    0,    0,    8,   10,   13,    0,    0,
   25,    0,   51,    0,    0,    0,    0,    0,    0,    0,
    0,   17,   45,   66,   67,    0,    0,   29,    0,   31,
    0,   27,   30,    0,    0,   49,    0,    0,    0,   58,
   62,   38,    7,    9,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   61,    0,   60,    0,    0,    0,   16,
   28,   46,   68,   69,    0,    0,    0,    0,    0,   11,
   23,    0,    0,    0,   37,    0,    0,   65,   73,    0,
    0,    0,   50,   53,    0,    0,   59,    0,    0,    0,
    0,    0,   64,    0,   48,    0,    0,   35,    0,   36,
   72,    0,    0,    0,    0,    0,   34,   32,    0,   39,
    0,    0,   40,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   43,    0,   44,   42,
};
#if defined(YYDESTRUCT_CALL) || defined(YYSTYPE_TOSTRING)
static const YYINT yystos[] = {                           0,
  263,  265,  267,  297,  298,  299,  300,  301,  302,  303,
  299,  256,  294,  307,  275,  279,  281,  282,  256,  276,
  303,  304,  287,  294,  277,  282,  305,  294,  276,  281,
  280,  279,  256,  257,  259,  260,  268,  269,  273,  274,
  275,  278,  282,  287,  288,  291,  294,  300,  303,  305,
  306,  308,  309,  310,  311,  312,  313,  314,  315,  316,
  317,  320,  282,  305,  303,  287,  275,  275,  275,  310,
  275,  275,  311,  316,  310,  316,  275,  279,  294,  278,
  308,  282,  284,  285,  286,  289,  290,  291,  293,  294,
  280,  310,  309,  310,  282,  310,  294,  276,  312,  318,
  319,  310,  312,  313,  314,  315,  316,  276,  309,  276,
  276,  276,  276,  281,  280,  308,  310,  308,  277,  282,
  312,  258,  276,  270,  321,  322,  308,  308,  287,  278,
  270,  271,  323,  283,  287,  283,  306,  283,  306,  262,
  306,  262,  282,  262,  282,  282,
};
#endif /* YYDESTRUCT_CALL || YYSTYPE_TOSTRING */
static const YYINT yydgoto[] = {                          4,
    5,    6,   48,    8,    9,   49,   22,   50,   51,   14,
   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,
  100,  101,   62,  125,  126,  133,
};
static const YYINT yysindex[] = {                        16,
    0,    0,    0,    0,   16,    0,    0,    0,    0, -252,
    0,    0, -248, -270,   75, -246, -236,    0,    0, -263,
 -232, -253, -256, -223, -132,    0,    0,    0, -238,   16,
    0, -209,    0, -194, -190, -186, -211, -182, -179,   41,
 -211,    0,    0,    0,    0,   41, -242,    0, -243,    0,
  -99,    0,    0, -183, -100,    0, -178, -222, -174,    0,
    0,    0,    0,    0, -162, -150, -211, -221, -211, -143,
 -211, -140, -164,    0, -113,    0,   41, -211, -135,    0,
    0,    0,    0,    0,   41,   41,   41,   41,   41,    0,
    0, -111, -221,  -89,    0,  -78,  -45,    0,    0,  -31,
 -104,  -70,    0,    0,  -79, -174,    0,   36, -211,   36,
  -34,  -32,    0,   41,    0,  -10,  -20,    0,  -13,    0,
    0,   36,   36,  -23,  -11, -118,    0,    0,  -14,    0,
    1,   -9,    0,   36,   -8,   36,  -66,   36,  -33,    5,
    3,    7,    0,   18,    0,    0,
};
static const YYINT yyrindex[] = {                         0,
    0,    0,    0,    0,  273,    0,    0,    0,    0,    0,
    0,    0,  -64,    0,    0,    0,    0,    0,    0,    0,
 -216,    0,    0,  -62,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   63,    0,    0,    0,
    0,    0,    0,    0, -142,    0,  -29, -260, -109,    0,
    0,    0,    0,    0, -197,    0,    0,    0,    0,    0,
    0,    0, -175,    0,    0,    0,   22,    0,  -64,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   26,    0,    0,    0,  -43,  -76,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -165,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   28,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,
};
#if YYBTYACC
static const YYINT yycindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,
};
#endif
static const YYINT yygindex[] = {                         0,
    0,  302,   82,    0,    0,   10,    0,  -12,  148,    0,
  -51,  -63,  -35,  -39,  -59,  222,  225,  226,  -37,    0,
    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 356
static const YYINT yytable[] = {                         81,
   73,   70,   74,   12,   93,   75,   73,   27,   76,   10,
   17,   18,   12,   25,   10,   54,   64,   99,   26,   54,
   54,   54,   29,   31,   21,  103,   15,   30,   54,  109,
   16,   92,   77,   94,   33,   96,   78,   73,   25,   65,
   23,   13,  102,   63,   33,   73,   73,   73,   73,   73,
   79,  107,   40,   41,  121,   32,  116,   24,  118,   14,
   43,   28,   40,   41,   14,   44,   45,   87,   88,   46,
  127,  128,   47,  117,   73,   44,   45,   66,   12,   46,
   67,    7,   47,   12,   68,   81,    7,   81,   69,   81,
   33,   33,   71,   33,   33,   72,   33,   33,   82,   33,
   63,   33,   33,   33,   63,   63,   63,   33,   33,   33,
   86,   33,   33,   63,   63,   63,   33,   63,   89,   83,
   84,   33,   33,   33,   34,   33,   35,   36,   33,   91,
    1,   90,    2,   63,    3,   37,   38,   63,   95,   63,
   39,   40,   41,   16,   25,   42,   63,   63,   63,   43,
   63,  131,  132,   97,   44,   45,   33,   34,   46,   35,
   36,   47,   98,    1,  108,    2,   56,    3,   37,   38,
   56,   56,   56,   39,   40,   41,  114,   25,   80,   56,
   56,   56,   43,   83,   84,   85,  110,   44,   45,   33,
   34,   46,   35,   36,   47,  140,    1,  111,    2,   57,
    3,   37,   38,   57,   57,   57,   39,   40,   41,  115,
   25,   88,   57,   57,   57,   43,   24,   24,   22,   22,
   44,   45,   33,   34,   46,   35,   36,   47,  142,    1,
  112,    2,   55,    3,   37,   38,   55,   55,   55,   39,
   40,   41,  119,   25,  113,   55,   52,  122,   43,  120,
   52,   52,   52,   44,   45,  123,  124,   46,   33,   34,
   47,   35,   36,  129,  144,    1,  130,    2,  134,    3,
   37,   38,    1,  136,  138,   39,   40,   41,    1,   25,
    2,  137,    3,  139,   43,  141,  143,  135,  145,   44,
   45,   33,   34,   46,   35,   36,   47,   71,    1,  146,
    2,   70,    3,   37,   38,   41,   11,  104,   39,   40,
   41,  105,   25,  106,   40,   41,    0,   43,    0,    0,
    0,    0,   44,   45,    0,    0,   46,   44,   45,   47,
   19,   46,    0,    0,   47,    0,    0,    1,   47,    2,
    0,    3,   47,   47,   47,    0,   47,   47,   47,    0,
   20,   47,   47,   47,    0,   47,
};
static const YYINT yycheck[] = {                         51,
   40,   37,   40,  256,   68,   41,   46,   20,   46,    0,
  281,  282,  256,  277,    5,  276,   29,   77,  282,  280,
  281,  282,  276,  280,   15,   85,  275,  281,  289,   93,
  279,   67,  275,   69,  256,   71,  279,   77,  277,   30,
  287,  294,   78,  282,  256,   85,   86,   87,   88,   89,
  294,   89,  274,  275,  114,  279,  108,  294,  110,  276,
  282,  294,  274,  275,  281,  287,  288,  290,  291,  291,
  122,  123,  294,  109,  114,  287,  288,  287,  276,  291,
  275,    0,  294,  281,  275,  137,    5,  139,  275,  141,
  256,  257,  275,  259,  260,  275,  262,  263,  282,  265,
  276,  267,  268,  269,  280,  281,  282,  273,  274,  275,
  289,  277,  278,  289,  290,  291,  282,  293,  293,  284,
  285,  287,  288,  256,  257,  291,  259,  260,  294,  280,
  263,  294,  265,  276,  267,  268,  269,  280,  282,  282,
  273,  274,  275,  279,  277,  278,  289,  290,  291,  282,
  293,  270,  271,  294,  287,  288,  256,  257,  291,  259,
  260,  294,  276,  263,  276,  265,  276,  267,  268,  269,
  280,  281,  282,  273,  274,  275,  281,  277,  278,  289,
  290,  291,  282,  284,  285,  286,  276,  287,  288,  256,
  257,  291,  259,  260,  294,  262,  263,  276,  265,  276,
  267,  268,  269,  280,  281,  282,  273,  274,  275,  280,
  277,  291,  289,  290,  291,  282,  281,  282,  281,  282,
  287,  288,  256,  257,  291,  259,  260,  294,  262,  263,
  276,  265,  276,  267,  268,  269,  280,  281,  282,  273,
  274,  275,  277,  277,  276,  289,  276,  258,  282,  282,
  280,  281,  282,  287,  288,  276,  270,  291,  256,  257,
  294,  259,  260,  287,  262,  263,  278,  265,  283,  267,
  268,  269,    0,  283,  283,  273,  274,  275,  263,  277,
  265,  134,  267,  136,  282,  138,  282,  287,  282,  287,
  288,  256,  257,  291,  259,  260,  294,  276,  263,  282,
  265,  276,  267,  268,  269,  278,    5,   86,  273,  274,
  275,   87,  277,   88,  274,  275,   -1,  282,   -1,   -1,
   -1,   -1,  287,  288,   -1,   -1,  291,  287,  288,  294,
  256,  291,   -1,   -1,  294,   -1,   -1,  263,  276,  265,
   -1,  267,  280,  281,  282,   -1,  284,  285,  286,   -1,
  276,  289,  290,  291,   -1,  293,
};
#if YYBTYACC
static const YYINT yyctable[] = {                        -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,
};
#endif
#define YYFINAL 4
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 295
#define YYUNDFTOKEN 324
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"$end",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"error","IF","ELSE","FOR","WHILE","DO",
"BREAK","INT","CHAR","FLOAT","DOUBLE","VOID","RETURN","SWITCH","CASE","DEFAULT",
"CONTINUE","PRINTLN","NOT","LPAREN","RPAREN","LCURL","RCURL","LTHIRD","RTHIRD",
"COMMA","SEMICOLON","COLON","INCOP","DECOP","ASSIGNOP","CONST_INT",
"CONST_FLOAT","LOGICOP","RELOP","ADDOP","BITOP","MULOP","ID","LOWER_THAN_ELSE",
"$accept","start","program","unit","var_declaration","func_declaration",
"func_definition","type_specifier","parameter_list","compound_statement",
"statements","declaration_list","statement","expression_statement","expression",
"variable","logic_expression","rel_expression","simple_expression","term",
"unary_expression","factor","argument_list","arguments","switch_declaration",
"switch_body","unit_body","default_body","illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : start",
"start : program",
"program : program unit",
"program : unit",
"unit : var_declaration",
"unit : func_declaration",
"unit : func_definition",
"func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON",
"func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON",
"func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement",
"func_definition : type_specifier ID LPAREN RPAREN compound_statement",
"parameter_list : parameter_list COMMA type_specifier ID",
"parameter_list : parameter_list COMMA type_specifier",
"parameter_list : type_specifier ID",
"parameter_list : type_specifier",
"parameter_list : error",
"compound_statement : LCURL statements RCURL",
"compound_statement : LCURL RCURL",
"var_declaration : type_specifier declaration_list SEMICOLON",
"type_specifier : INT",
"type_specifier : FLOAT",
"type_specifier : VOID",
"declaration_list : declaration_list COMMA ID",
"declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD",
"declaration_list : ID",
"declaration_list : ID LTHIRD CONST_INT RTHIRD",
"declaration_list : error",
"statements : statement",
"statements : statements statement",
"statement : var_declaration",
"statement : expression_statement",
"statement : compound_statement",
"statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement",
"statement : IF LPAREN expression RPAREN statement",
"statement : IF LPAREN expression RPAREN statement ELSE statement",
"statement : WHILE LPAREN expression RPAREN statement",
"statement : PRINTLN LPAREN ID RPAREN SEMICOLON",
"statement : RETURN expression SEMICOLON",
"statement : switch_declaration",
"switch_declaration : SWITCH LPAREN expression RPAREN LCURL switch_body RCURL",
"switch_body : unit_body default_body",
"switch_body : unit_body",
"unit_body : unit_body CASE CONST_INT COLON statements BREAK SEMICOLON",
"unit_body : CASE CONST_INT COLON statements BREAK SEMICOLON",
"default_body : DEFAULT COLON statements BREAK SEMICOLON",
"expression_statement : SEMICOLON",
"expression_statement : expression SEMICOLON",
"variable : ID",
"variable : ID LTHIRD expression RTHIRD",
"expression : logic_expression",
"expression : variable ASSIGNOP logic_expression",
"expression : error",
"logic_expression : rel_expression",
"logic_expression : rel_expression LOGICOP rel_expression",
"rel_expression : simple_expression",
"rel_expression : simple_expression RELOP simple_expression",
"simple_expression : term",
"simple_expression : simple_expression ADDOP term",
"term : unary_expression",
"term : term MULOP unary_expression",
"unary_expression : ADDOP unary_expression",
"unary_expression : NOT unary_expression",
"unary_expression : factor",
"factor : variable",
"factor : ID LPAREN argument_list RPAREN",
"factor : LPAREN expression RPAREN",
"factor : CONST_INT",
"factor : CONST_FLOAT",
"factor : variable INCOP",
"factor : variable DECOP",
"argument_list : arguments",
"argument_list :",
"arguments : arguments COMMA logic_expression",
"arguments : logic_expression",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;
int      yynerrs;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE  yyloc; /* position returned by actions */
YYLTYPE  yylloc; /* position from the lexer */
#endif

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(loc, rhs, n) \
do \
{ \
    if (n == 0) \
    { \
        (loc).first_line   = YYRHSLOC(rhs, 0).last_line; \
        (loc).first_column = YYRHSLOC(rhs, 0).last_column; \
        (loc).last_line    = YYRHSLOC(rhs, 0).last_line; \
        (loc).last_column  = YYRHSLOC(rhs, 0).last_column; \
    } \
    else \
    { \
        (loc).first_line   = YYRHSLOC(rhs, 1).first_line; \
        (loc).first_column = YYRHSLOC(rhs, 1).first_column; \
        (loc).last_line    = YYRHSLOC(rhs, n).last_line; \
        (loc).last_column  = YYRHSLOC(rhs, n).last_column; \
    } \
} while (0)
#endif /* YYLLOC_DEFAULT */
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#if YYBTYACC

#ifndef YYLVQUEUEGROWTH
#define YYLVQUEUEGROWTH 32
#endif
#endif /* YYBTYACC */

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#ifndef YYINITSTACKSIZE
#define YYINITSTACKSIZE 200
#endif

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE  *p_base;
    YYLTYPE  *p_mark;
#endif
} YYSTACKDATA;
#if YYBTYACC

struct YYParseState_s
{
    struct YYParseState_s *save;    /* Previously saved parser state */
    YYSTACKDATA            yystack; /* saved parser stack */
    int                    state;   /* saved parser state */
    int                    errflag; /* saved error recovery status */
    int                    lexeme;  /* saved index of the conflict lexeme in the lexical queue */
    YYINT                  ctry;    /* saved index in yyctable[] for this conflict */
};
typedef struct YYParseState_s YYParseState;
#endif /* YYBTYACC */
/* variables for the parser stack */
static YYSTACKDATA yystack;
#if YYBTYACC

/* Current parser state */
static YYParseState *yyps = 0;

/* yypath != NULL: do the full parse, starting at *yypath parser state. */
static YYParseState *yypath = 0;

/* Base of the lexical value queue */
static YYSTYPE *yylvals = 0;

/* Current position at lexical value queue */
static YYSTYPE *yylvp = 0;

/* End position of lexical value queue */
static YYSTYPE *yylve = 0;

/* The last allocated position at the lexical value queue */
static YYSTYPE *yylvlim = 0;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
/* Base of the lexical position queue */
static YYLTYPE *yylpsns = 0;

/* Current position at lexical position queue */
static YYLTYPE *yylpp = 0;

/* End position of lexical position queue */
static YYLTYPE *yylpe = 0;

/* The last allocated position at the lexical position queue */
static YYLTYPE *yylplim = 0;
#endif

/* Current position at lexical token queue */
static YYINT  *yylexp = 0;

static YYINT  *yylexemes = 0;
#endif /* YYBTYACC */
#line 1583 "2005037.y"
int main(int argc,char *argv[])
{
	FILE *fp ;

	// fp = fopen("input.txt", "r");

    if((fp=fopen(argv[1],"r"))==NULL)
	{
		printf("Cannot Open Input File.\n");
		exit(1);
	}

	logFile.open("2005037_log.txt");
	errorFile.open("2005037_error.txt");
	parseFile.open("2005037_parsetree.txt");
	assemblyFile.open("2005037_assembly.txt");
	

	yyin=fp;
	yyparse();
	

	logFile.close();
	errorFile.close();
	parseFile.close();
	assemblyFile.close();
	fclose(fp);
	
	return 0;
}

#line 1325 "y.tab.c"

/* For use in generated program */
#define yydepth (int)(yystack.s_mark - yystack.s_base)
#if YYBTYACC
#define yytrial (yyps->save)
#endif /* YYBTYACC */

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE *newps;
#endif

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    newps = (YYLTYPE *)realloc(data->p_base, newsize * sizeof(*newps));
    if (newps == 0)
        return YYENOMEM;

    data->p_base = newps;
    data->p_mark = newps + i;
#endif

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;

#if YYDEBUG
    if (yydebug)
        fprintf(stderr, "%sdebug: stack size increased to %d\n", YYPREFIX, newsize);
#endif
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    free(data->p_base);
#endif
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif /* YYPURE || defined(YY_NO_LEAKS) */
#if YYBTYACC

static YYParseState *
yyNewState(unsigned size)
{
    YYParseState *p = (YYParseState *) malloc(sizeof(YYParseState));
    if (p == NULL) return NULL;

    p->yystack.stacksize = size;
    if (size == 0)
    {
        p->yystack.s_base = NULL;
        p->yystack.l_base = NULL;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        p->yystack.p_base = NULL;
#endif
        return p;
    }
    p->yystack.s_base    = (YYINT *) malloc(size * sizeof(YYINT));
    if (p->yystack.s_base == NULL) return NULL;
    p->yystack.l_base    = (YYSTYPE *) malloc(size * sizeof(YYSTYPE));
    if (p->yystack.l_base == NULL) return NULL;
    memset(p->yystack.l_base, 0, size * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    p->yystack.p_base    = (YYLTYPE *) malloc(size * sizeof(YYLTYPE));
    if (p->yystack.p_base == NULL) return NULL;
    memset(p->yystack.p_base, 0, size * sizeof(YYLTYPE));
#endif

    return p;
}

static void
yyFreeState(YYParseState *p)
{
    yyfreestack(&p->yystack);
    free(p);
}
#endif /* YYBTYACC */

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab
#if YYBTYACC
#define YYVALID        do { if (yyps->save)            goto yyvalid; } while(0)
#define YYVALID_NESTED do { if (yyps->save && \
                                yyps->save->save == 0) goto yyvalid; } while(0)
#endif /* YYBTYACC */

int
YYPARSE_DECL()
{
    int yym, yyn, yystate, yyresult;
#if YYBTYACC
    int yynewerrflag;
    YYParseState *yyerrctx = NULL;
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE  yyerror_loc_range[3]; /* position of error start/end (0 unused) */
#endif
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
    if (yydebug)
        fprintf(stderr, "%sdebug[<# of symbols on state stack>]\n", YYPREFIX);
#endif
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    memset(yyerror_loc_range, 0, sizeof(yyerror_loc_range));
#endif

#if YYBTYACC
    yyps = yyNewState(0); if (yyps == 0) goto yyenomem;
    yyps->save = 0;
#endif /* YYBTYACC */
    yym = 0;
    /* yyn is set below */
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark = yystack.p_base;
#endif
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
#if YYBTYACC
        do {
        if (yylvp < yylve)
        {
            /* we're currently re-reading tokens */
            yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylloc = *yylpp++;
#endif
            yychar = *yylexp++;
            break;
        }
        if (yyps->save)
        {
            /* in trial mode; save scanner results for future parse attempts */
            if (yylvp == yylvlim)
            {   /* Enlarge lexical value queue */
                size_t p = (size_t) (yylvp - yylvals);
                size_t s = (size_t) (yylvlim - yylvals);

                s += YYLVQUEUEGROWTH;
                if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL) goto yyenomem;
                if ((yylvals   = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL) goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                if ((yylpsns   = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL) goto yyenomem;
#endif
                yylvp   = yylve = yylvals + p;
                yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp   = yylpe = yylpsns + p;
                yylplim = yylpsns + s;
#endif
                yylexp  = yylexemes + p;
            }
            *yylexp = (YYINT) YYLEX;
            *yylvp++ = yylval;
            yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            *yylpp++ = yylloc;
            yylpe++;
#endif
            yychar = *yylexp++;
            break;
        }
        /* normal operation, no conflict encountered */
#endif /* YYBTYACC */
        yychar = YYLEX;
#if YYBTYACC
        } while (0);
#endif /* YYBTYACC */
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)",
                            YYDEBUGSTR, yydepth, yystate, yychar, yys);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
            if (!yytrial)
#endif /* YYBTYACC */
                fprintf(stderr, " <%s>", YYSTYPE_TOSTRING(yychar, yylval));
#endif
            fputc('\n', stderr);
        }
#endif
    }
#if YYBTYACC

    /* Do we have a conflict? */
    if (((yyn = yycindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
        yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        YYINT ctry;

        if (yypath)
        {
            YYParseState *save;
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%s[%d]: CONFLICT in state %d: following successful trial parse\n",
                                YYDEBUGSTR, yydepth, yystate);
#endif
            /* Switch to the next conflict context */
            save = yypath;
            yypath = save->save;
            save->save = NULL;
            ctry = save->ctry;
            if (save->state != yystate) YYABORT;
            yyFreeState(save);

        }
        else
        {

            /* Unresolved conflict - start/continue trial parse */
            YYParseState *save;
#if YYDEBUG
            if (yydebug)
            {
                fprintf(stderr, "%s[%d]: CONFLICT in state %d. ", YYDEBUGSTR, yydepth, yystate);
                if (yyps->save)
                    fputs("ALREADY in conflict, continuing trial parse.\n", stderr);
                else
                    fputs("Starting trial parse.\n", stderr);
            }
#endif
            save                  = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
            if (save == NULL) goto yyenomem;
            save->save            = yyps->save;
            save->state           = yystate;
            save->errflag         = yyerrflag;
            save->yystack.s_mark  = save->yystack.s_base + (yystack.s_mark - yystack.s_base);
            memcpy (save->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            save->yystack.l_mark  = save->yystack.l_base + (yystack.l_mark - yystack.l_base);
            memcpy (save->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            save->yystack.p_mark  = save->yystack.p_base + (yystack.p_mark - yystack.p_base);
            memcpy (save->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            ctry                  = yytable[yyn];
            if (yyctable[ctry] == -1)
            {
#if YYDEBUG
                if (yydebug && yychar >= YYEOF)
                    fprintf(stderr, "%s[%d]: backtracking 1 token\n", YYDEBUGSTR, yydepth);
#endif
                ctry++;
            }
            save->ctry = ctry;
            if (yyps->save == NULL)
            {
                /* If this is a first conflict in the stack, start saving lexemes */
                if (!yylexemes)
                {
                    yylexemes = (YYINT *) malloc((YYLVQUEUEGROWTH) * sizeof(YYINT));
                    if (yylexemes == NULL) goto yyenomem;
                    yylvals   = (YYSTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYSTYPE));
                    if (yylvals == NULL) goto yyenomem;
                    yylvlim   = yylvals + YYLVQUEUEGROWTH;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpsns   = (YYLTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYLTYPE));
                    if (yylpsns == NULL) goto yyenomem;
                    yylplim   = yylpsns + YYLVQUEUEGROWTH;
#endif
                }
                if (yylvp == yylve)
                {
                    yylvp  = yylve = yylvals;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpp  = yylpe = yylpsns;
#endif
                    yylexp = yylexemes;
                    if (yychar >= YYEOF)
                    {
                        *yylve++ = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                        *yylpe++ = yylloc;
#endif
                        *yylexp  = (YYINT) yychar;
                        yychar   = YYEMPTY;
                    }
                }
            }
            if (yychar >= YYEOF)
            {
                yylvp--;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp--;
#endif
                yylexp--;
                yychar = YYEMPTY;
            }
            save->lexeme = (int) (yylvp - yylvals);
            yyps->save   = save;
        }
        if (yytable[yyn] == ctry)
        {
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                                YYDEBUGSTR, yydepth, yystate, yyctable[ctry]);
#endif
            if (yychar < 0)
            {
                yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp++;
#endif
                yylexp++;
            }
            if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                goto yyoverflow;
            yystate = yyctable[ctry];
            *++yystack.s_mark = (YYINT) yystate;
            *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            *++yystack.p_mark = yylloc;
#endif
            yychar  = YYEMPTY;
            if (yyerrflag > 0) --yyerrflag;
            goto yyloop;
        }
        else
        {
            yyn = yyctable[ctry];
            goto yyreduce;
        }
    } /* End of code dealing with conflicts */
#endif /* YYBTYACC */
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                            YYDEBUGSTR, yydepth, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        *++yystack.p_mark = yylloc;
#endif
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;
#if YYBTYACC

    yynewerrflag = 1;
    goto yyerrhandler;
    goto yyerrlab; /* redundant goto avoids 'unused label' warning */

yyerrlab:
    /* explicit YYERROR from an action -- pop the rhs of the rule reduced
     * before looking for error recovery */
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark -= yym;
#endif

    yynewerrflag = 0;
yyerrhandler:
    while (yyps->save)
    {
        int ctry;
        YYParseState *save = yyps->save;
#if YYDEBUG
        if (yydebug)
            fprintf(stderr, "%s[%d]: ERROR in state %d, CONFLICT BACKTRACKING to state %d, %d tokens\n",
                            YYDEBUGSTR, yydepth, yystate, yyps->save->state,
                    (int)(yylvp - yylvals - yyps->save->lexeme));
#endif
        /* Memorize most forward-looking error state in case it's really an error. */
        if (yyerrctx == NULL || yyerrctx->lexeme < yylvp - yylvals)
        {
            /* Free old saved error context state */
            if (yyerrctx) yyFreeState(yyerrctx);
            /* Create and fill out new saved error context state */
            yyerrctx                 = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
            if (yyerrctx == NULL) goto yyenomem;
            yyerrctx->save           = yyps->save;
            yyerrctx->state          = yystate;
            yyerrctx->errflag        = yyerrflag;
            yyerrctx->yystack.s_mark = yyerrctx->yystack.s_base + (yystack.s_mark - yystack.s_base);
            memcpy (yyerrctx->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            yyerrctx->yystack.l_mark = yyerrctx->yystack.l_base + (yystack.l_mark - yystack.l_base);
            memcpy (yyerrctx->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yyerrctx->yystack.p_mark = yyerrctx->yystack.p_base + (yystack.p_mark - yystack.p_base);
            memcpy (yyerrctx->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            yyerrctx->lexeme         = (int) (yylvp - yylvals);
        }
        yylvp          = yylvals   + save->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        yylpp          = yylpsns   + save->lexeme;
#endif
        yylexp         = yylexemes + save->lexeme;
        yychar         = YYEMPTY;
        yystack.s_mark = yystack.s_base + (save->yystack.s_mark - save->yystack.s_base);
        memcpy (yystack.s_base, save->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
        yystack.l_mark = yystack.l_base + (save->yystack.l_mark - save->yystack.l_base);
        memcpy (yystack.l_base, save->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        yystack.p_mark = yystack.p_base + (save->yystack.p_mark - save->yystack.p_base);
        memcpy (yystack.p_base, save->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
        ctry           = ++save->ctry;
        yystate        = save->state;
        /* We tried shift, try reduce now */
        if ((yyn = yyctable[ctry]) >= 0) goto yyreduce;
        yyps->save     = save->save;
        save->save     = NULL;
        yyFreeState(save);

        /* Nothing left on the stack -- error */
        if (!yyps->save)
        {
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%sdebug[%d,trial]: trial parse FAILED, entering ERROR mode\n",
                                YYPREFIX, yydepth);
#endif
            /* Restore state as it was in the most forward-advanced error */
            yylvp          = yylvals   + yyerrctx->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylpp          = yylpsns   + yyerrctx->lexeme;
#endif
            yylexp         = yylexemes + yyerrctx->lexeme;
            yychar         = yylexp[-1];
            yylval         = yylvp[-1];
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylloc         = yylpp[-1];
#endif
            yystack.s_mark = yystack.s_base + (yyerrctx->yystack.s_mark - yyerrctx->yystack.s_base);
            memcpy (yystack.s_base, yyerrctx->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            yystack.l_mark = yystack.l_base + (yyerrctx->yystack.l_mark - yyerrctx->yystack.l_base);
            memcpy (yystack.l_base, yyerrctx->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yystack.p_mark = yystack.p_base + (yyerrctx->yystack.p_mark - yyerrctx->yystack.p_base);
            memcpy (yystack.p_base, yyerrctx->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            yystate        = yyerrctx->state;
            yyFreeState(yyerrctx);
            yyerrctx       = NULL;
        }
        yynewerrflag = 1;
    }
    if (yynewerrflag == 0) goto yyinrecovery;
#endif /* YYBTYACC */

    YYERROR_CALL("syntax error");
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yyerror_loc_range[1] = yylloc; /* lookahead position is error start position */
#endif

#if !YYBTYACC
    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
#endif
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    fprintf(stderr, "%s[%d]: state %d, error recovery shifting to state %d\n",
                                    YYDEBUGSTR, yydepth, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                /* lookahead position is error end position */
                yyerror_loc_range[2] = yylloc;
                YYLLOC_DEFAULT(yyloc, yyerror_loc_range, 2); /* position of error span */
                *++yystack.p_mark = yyloc;
#endif
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    fprintf(stderr, "%s[%d]: error recovery discarding state %d\n",
                                    YYDEBUGSTR, yydepth, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                /* the current TOS position is the error start position */
                yyerror_loc_range[1] = *yystack.p_mark;
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
                if (!yytrial)
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    YYDESTRUCT_CALL("error: discarding state",
                                    yystos[*yystack.s_mark], yystack.l_mark, yystack.p_mark);
#else
                    YYDESTRUCT_CALL("error: discarding state",
                                    yystos[*yystack.s_mark], yystack.l_mark);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#endif /* defined(YYDESTRUCT_CALL) */
                --yystack.s_mark;
                --yystack.l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                --yystack.p_mark;
#endif
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, error recovery discarding token %d (%s)\n",
                            YYDEBUGSTR, yydepth, yystate, yychar, yys);
        }
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            YYDESTRUCT_CALL("error: discarding token", yychar, &yylval, &yylloc);
#else
            YYDESTRUCT_CALL("error: discarding token", yychar, &yylval);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#endif /* defined(YYDESTRUCT_CALL) */
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
    yym = yylen[yyn];
#if YYDEBUG
    if (yydebug)
    {
        fprintf(stderr, "%s[%d]: state %d, reducing by rule %d (%s)",
                        YYDEBUGSTR, yydepth, yystate, yyn, yyrule[yyn]);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
            if (yym > 0)
            {
                int i;
                fputc('<', stderr);
                for (i = yym; i > 0; i--)
                {
                    if (i != yym) fputs(", ", stderr);
                    fputs(YYSTYPE_TOSTRING(yystos[yystack.s_mark[1-i]],
                                           yystack.l_mark[1-i]), stderr);
                }
                fputc('>', stderr);
            }
#endif
        fputc('\n', stderr);
    }
#endif
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)

    /* Perform position reduction */
    memset(&yyloc, 0, sizeof(yyloc));
#if YYBTYACC
    if (!yytrial)
#endif /* YYBTYACC */
    {
        YYLLOC_DEFAULT(yyloc, &yystack.p_mark[-yym], yym);
        /* just in case YYERROR is invoked within the action, save
           the start of the rhs as the error start position */
        yyerror_loc_range[1] = yystack.p_mark[1-yym];
    }
#endif

    switch (yyn)
    {
case 1:
#line 740 "2005037.y"
	{
		logFileWriter("start","program");
		summaryWriter();
		yyval.parseTreeNode = new ParseTreeNode("start");
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		printParseTree(yyval.parseTreeNode,0);
		codeGenerator(yyval.parseTreeNode);
	}
#line 2005 "y.tab.c"
break;
case 2:
#line 750 "2005037.y"
	{
	logFileWriter("program","program unit");
	yyval.parseTreeNode = new ParseTreeNode("program");
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
}
#line 2015 "y.tab.c"
break;
case 3:
#line 756 "2005037.y"
	{logFileWriter("program","unit");
	
	yyval.parseTreeNode = new ParseTreeNode("program");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	
	}
#line 2025 "y.tab.c"
break;
case 4:
#line 764 "2005037.y"
	{
	logFileWriter("unit","var_declaration");
	yyval.parseTreeNode = new ParseTreeNode("unit");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
}
#line 2034 "y.tab.c"
break;
case 5:
#line 769 "2005037.y"
	{
		logFileWriter("unit","func_declaration");
		yyval.parseTreeNode = new ParseTreeNode("unit");
	    yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		definingFunction= false ;
	 }
#line 2044 "y.tab.c"
break;
case 6:
#line 775 "2005037.y"
	{
		logFileWriter("unit","func_definition");
		yyval.parseTreeNode = new ParseTreeNode("unit");
	    yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		definingFunction= true;
	 }
#line 2054 "y.tab.c"
break;
case 7:
#line 783 "2005037.y"
	{
	logFileWriter("func_declaration","type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
    yyval.parseTreeNode = new ParseTreeNode("func_declaration");
	yyval.parseTreeNode->addChild(yystack.l_mark[-5].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
    
	funcName = yystack.l_mark[-4].parseTreeNode->lexeme ;
	int discoveryLine = yystack.l_mark[-4].parseTreeNode->startLine;

	funcReturnType = yystack.l_mark[-5].parseTreeNode->lastFoundLexeme ;
	handleFunctionDeclaration(funcName,toUpper(funcReturnType),discoveryLine);

}
#line 2075 "y.tab.c"
break;
case 8:
#line 800 "2005037.y"
	{
			logFileWriter("func_declaration","type_specifier ID LPAREN RPAREN SEMICOLON");
		    yyval.parseTreeNode = new ParseTreeNode("func_declaration");
			yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

	         funcName = yystack.l_mark[-3].parseTreeNode->lexeme ;
	         int discoveryLine = yystack.l_mark[-3].parseTreeNode->startLine;
			 funcReturnType = yystack.l_mark[-4].parseTreeNode->lastFoundLexeme ;
	        handleFunctionDeclaration(funcName,toUpper(funcReturnType),discoveryLine);
		}
#line 2093 "y.tab.c"
break;
case 9:
#line 816 "2005037.y"
	{
	logFileWriter("func_definition","type_specifier ID LPAREN parameter_list RPAREN compound_statement");
   
    yyval.parseTreeNode = new ParseTreeNode("func_definition");
	 
	yyval.parseTreeNode->addChild(yystack.l_mark[-5].parseTreeNode);
	/*cout<<"child 1 inserted"<<endl;*/
	yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
	/*cout<<"Let's see child 2 "<<$2->lexeme<<endl;*/
	/*cout<<"child 2 inserted"<<endl;*/
	yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
	/*cout<<"child 3 inserted"<<endl;*/
	yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	/*cout<<"child 4 inserted"<<endl;*/
    yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	/*cout<<"child 5 inserted"<<endl;*/
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	/*cout<<"child 6 inserted"<<endl;*/
    
	funcName = yystack.l_mark[-4].parseTreeNode->lexeme ;
	int discoveryLine = yystack.l_mark[-4].parseTreeNode->startLine;
	funcReturnType = yystack.l_mark[-5].parseTreeNode->lastFoundLexeme ;
	/*cout<<"My return type is "<<funcReturnType<<endl;*/
	if(!isError){
		handleFunctionDeclaration(funcName,toUpper(funcReturnType),discoveryLine);
    }else{
		isError = false ;
	}
	
	
}
#line 2128 "y.tab.c"
break;
case 10:
#line 847 "2005037.y"
	{
			logFileWriter("func_definition","type_specifier ID LPAREN RPAREN compound_statement");
		    yyval.parseTreeNode = new ParseTreeNode("func_definition");
			yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		
		    funcName = yystack.l_mark[-3].parseTreeNode->lexeme ;
	        int discoveryLine = yystack.l_mark[-3].parseTreeNode->startLine;
	        funcReturnType = yystack.l_mark[-4].parseTreeNode->lastFoundLexeme ;
			/*cout<<"My return type is "<<funcReturnType<<endl;*/
	        handleFunctionDeclaration(funcName,toUpper(funcReturnType),discoveryLine);

		}
#line 2148 "y.tab.c"
break;
case 11:
#line 866 "2005037.y"
	{
	logFileWriter("parameter_list","parameter_list COMMA type_specifier ID");
	yyval.parseTreeNode = new ParseTreeNode("parameter_list");
	yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

    string paramName = yystack.l_mark[0].parseTreeNode->lexeme ;
	/*now we'll insert varType and lexeme in our paramList*/
	parameters.push_back(make_pair(toUpper(varType),paramName));
    

	}
#line 2166 "y.tab.c"
break;
case 12:
#line 880 "2005037.y"
	{
			logFileWriter("parameter_list","parameter_list COMMA type_specifier");
			yyval.parseTreeNode = new ParseTreeNode("parameter_list");
			yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		    yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		    yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

			parameters.push_back(make_pair(toUpper(varType),""));

			}
#line 2180 "y.tab.c"
break;
case 13:
#line 890 "2005037.y"
	{
			logFileWriter("parameter_list","type_specifier ID");
			yyval.parseTreeNode = new ParseTreeNode("parameter_list");
			yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		    yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

			string paramName = yystack.l_mark[0].parseTreeNode->lexeme;

			parameters.push_back(make_pair(toUpper(varType),paramName));

			}
#line 2195 "y.tab.c"
break;
case 14:
#line 901 "2005037.y"
	{
			logFileWriter("parameter_list","type_specifier");
			yyval.parseTreeNode = new ParseTreeNode("parameter_list");
			yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
			parameters.push_back(make_pair(toUpper(varType),""));
			}
#line 2205 "y.tab.c"
break;
case 15:
#line 907 "2005037.y"
	{
			/* attempting to catch syntax error */
			yyclearin;
			isError = true;
			yyval.parseTreeNode = new ParseTreeNode("error","parameter_list",line);
			errorFileWriter("Syntax error at parameter list of function definition",line);
			logFile<<"Error at line no "<<line<<" : Syntax error"<<endl;

		}
#line 2218 "y.tab.c"
break;
case 16:
#line 919 "2005037.y"
	{
	logFileWriter("compound_statement","LCURL statements RCURL");
    yyval.parseTreeNode = new ParseTreeNode("compound_statement");
	yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	yyval.parseTreeNode->offset = yystack.l_mark[-1].parseTreeNode->offset ;
	printScopeTable();
	symbolTable.exitScope();
}
#line 2232 "y.tab.c"
break;
case 17:
#line 929 "2005037.y"
	{
				logFileWriter("compound_statement","LCURL RCURL");
			    yyval.parseTreeNode = new ParseTreeNode("compound_statement");
				yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
				yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		        printScopeTable();
	            symbolTable.exitScope();
			}
#line 2244 "y.tab.c"
break;
case 18:
#line 939 "2005037.y"
	{logFileWriter("var_declaration","type_specifier declaration_list SEMICOLON");

yyval.parseTreeNode = new ParseTreeNode("var_declaration");
yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
yyval.parseTreeNode->offset = offset ;

}
#line 2257 "y.tab.c"
break;
case 19:
#line 950 "2005037.y"
	{
							logFileWriter("type_specifier","INT");
							varType = "INT" ;
							yyval.parseTreeNode = new ParseTreeNode("type_specifier");
							yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);


					   }
#line 2269 "y.tab.c"
break;
case 20:
#line 958 "2005037.y"
	{
							logFileWriter("type_specifier", "FLOAT");
							varType = "FLOAT" ;
							yyval.parseTreeNode = new ParseTreeNode("type_specifier");
							yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
					   }
#line 2279 "y.tab.c"
break;
case 21:
#line 964 "2005037.y"
	{
							logFileWriter("type_specifier", "VOID");
							varType = "VOID" ;
							yyval.parseTreeNode = new ParseTreeNode("type_specifier");
							yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		}
#line 2289 "y.tab.c"
break;
case 22:
#line 972 "2005037.y"
	{
	logFileWriter("declaration_list","declaration_list COMMA ID");
	
	handleIdDeclaration(yystack.l_mark[0].parseTreeNode,-1);

	yyval.parseTreeNode = new ParseTreeNode("declaration_list");

	yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	

}
#line 2306 "y.tab.c"
break;
case 23:
#line 985 "2005037.y"
	{
			logFileWriter("declaration_list","declaration_list COMMA ID LSQUARE CONST_INT RSQUARE");
			yyval.parseTreeNode = new ParseTreeNode("declaration_list");
			yyval.parseTreeNode->addChild(yystack.l_mark[-5].parseTreeNode);
		    yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
		    yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
		    yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		    yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		    yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

			int size = stoi(yystack.l_mark[-1].parseTreeNode->lexeme) ;
			/*cout<<"found array size "<<size<<endl;*/
            handleIdDeclaration(yystack.l_mark[-3].parseTreeNode,size);
			
		  }
#line 2325 "y.tab.c"
break;
case 24:
#line 1000 "2005037.y"
	{
			logFileWriter("declaration_list","ID");
			handleIdDeclaration(yystack.l_mark[0].parseTreeNode,-1); /* -1 indicating that it's not an array*/
			cout<<offset<<endl;
			yyval.parseTreeNode = new ParseTreeNode("declaration_list");
			yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
			
			}
#line 2337 "y.tab.c"
break;
case 25:
#line 1008 "2005037.y"
	{
			logFileWriter("declaration_list","ID LSQUARE CONST_INT RSQUARE");
			yyval.parseTreeNode = new ParseTreeNode("declaration_list");
			yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
			yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
			yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
			yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

			int size = stoi(yystack.l_mark[-1].parseTreeNode->lexeme) ;
			/*cout<<"found array size "<<size<<endl;*/
            handleIdDeclaration(yystack.l_mark[-3].parseTreeNode,size);
		  }
#line 2353 "y.tab.c"
break;
case 26:
#line 1021 "2005037.y"
	{
			yyclearin;
			hasDeclListError = true;
			yyval.parseTreeNode = new ParseTreeNode("error","declaration_list",line);
			errorFileWriter("Syntax error at declaration list of variable declaration",line);
			logFile<<"Error at line no "<<line<<" : Syntax error"<<endl;
		  }
#line 2364 "y.tab.c"
break;
case 27:
#line 1030 "2005037.y"
	{logFileWriter("statements","statement");
    yyval.parseTreeNode = new ParseTreeNode("statements");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	yyval.parseTreeNode->offset = yystack.l_mark[0].parseTreeNode->offset ;
}
#line 2373 "y.tab.c"
break;
case 28:
#line 1035 "2005037.y"
	{
		logFileWriter("statements","statements statement");
	
	    yyval.parseTreeNode = new ParseTreeNode("statements");
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);   
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);   
		yyval.parseTreeNode->offset = yystack.l_mark[0].parseTreeNode->offset ;
	   }
#line 2385 "y.tab.c"
break;
case 29:
#line 1045 "2005037.y"
	{
	logFileWriter("statement","var_declaration");
    yyval.parseTreeNode = new ParseTreeNode("statement");
    yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	yyval.parseTreeNode->offset = yystack.l_mark[0].parseTreeNode->offset ;
}
#line 2395 "y.tab.c"
break;
case 30:
#line 1051 "2005037.y"
	{
		logFileWriter("statement","expression_statement");
	    yyval.parseTreeNode = new ParseTreeNode("statement");
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	  }
#line 2404 "y.tab.c"
break;
case 31:
#line 1056 "2005037.y"
	{
		logFileWriter("statement","compound_statement");
	    yyval.parseTreeNode = new ParseTreeNode("statement");
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		symbolTable.exitScope();
	  }
#line 2414 "y.tab.c"
break;
case 32:
#line 1062 "2005037.y"
	{
		logFileWriter("statement","FOR LPAREN expression_statement expression_statement expression RPAREN statement");
	    yyval.parseTreeNode = new ParseTreeNode("statement");
		yyval.parseTreeNode->addChild(yystack.l_mark[-6].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-5].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	  }
#line 2429 "y.tab.c"
break;
case 33:
#line 1073 "2005037.y"
	{
		logFileWriter("statement","IF LPAREN expression RPAREN statement");
	    yyval.parseTreeNode = new ParseTreeNode("statement");
		yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	  }
#line 2442 "y.tab.c"
break;
case 34:
#line 1082 "2005037.y"
	{
		logFileWriter("statement","IF LAPAREN expression RPAREN statement ELSE statement");
	    yyval.parseTreeNode = new ParseTreeNode("statement");
		yyval.parseTreeNode->addChild(yystack.l_mark[-6].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-5].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	  }
#line 2457 "y.tab.c"
break;
case 35:
#line 1093 "2005037.y"
	{
		logFileWriter("statement","WHILE LPAREN expression RPAREN statement");
	    yyval.parseTreeNode = new ParseTreeNode("statement");
		yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	  }
#line 2470 "y.tab.c"
break;
case 36:
#line 1104 "2005037.y"
	{
		logFileWriter("statement","PRINTLN LPAREN ID RPAREN SEMICOLON");
	    yyval.parseTreeNode = new ParseTreeNode("statement");
		yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	  }
#line 2483 "y.tab.c"
break;
case 37:
#line 1113 "2005037.y"
	{
		logFileWriter("statement","RETURN expression SEMICOLON");
	    yyval.parseTreeNode = new ParseTreeNode("statement");
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	  }
#line 2494 "y.tab.c"
break;
case 38:
#line 1120 "2005037.y"
	{
				
				yyval.parseTreeNode = new ParseTreeNode("statement");
				yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
			}
#line 2503 "y.tab.c"
break;
case 39:
#line 1127 "2005037.y"
	{

	yyval.parseTreeNode = new ParseTreeNode("switch_declaration");
	yyval.parseTreeNode->addChild(yystack.l_mark[-6].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-5].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);


}
#line 2520 "y.tab.c"
break;
case 40:
#line 1142 "2005037.y"
	{

	yyval.parseTreeNode = new ParseTreeNode("switch_body");
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

                 }
#line 2531 "y.tab.c"
break;
case 41:
#line 1149 "2005037.y"
	{
					yyval.parseTreeNode = new ParseTreeNode("switch_body");
					yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
				 }
#line 2539 "y.tab.c"
break;
case 42:
#line 1155 "2005037.y"
	{

	yyval.parseTreeNode = new ParseTreeNode("unit_body");
	yyval.parseTreeNode->addChild(yystack.l_mark[-6].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-5].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

}
#line 2555 "y.tab.c"
break;
case 43:
#line 1167 "2005037.y"
	{

			yyval.parseTreeNode = new ParseTreeNode("unit_body");
			yyval.parseTreeNode->addChild(yystack.l_mark[-5].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	        yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);


		  }
#line 2571 "y.tab.c"
break;
case 44:
#line 1181 "2005037.y"
	{

	yyval.parseTreeNode = new ParseTreeNode("default_body");
    yyval.parseTreeNode->addChild(yystack.l_mark[-4].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

							}
#line 2585 "y.tab.c"
break;
case 45:
#line 1194 "2005037.y"
	{
	logFileWriter("expression_statement","SEMICOLON");
    yyval.parseTreeNode = new ParseTreeNode("expression_statement");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

}
#line 2595 "y.tab.c"
break;
case 46:
#line 1200 "2005037.y"
	{
				logFileWriter("expression_statement","expression SEMICOLON");
			    yyval.parseTreeNode = new ParseTreeNode("expression_statement");
				yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
				yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
			
			}
#line 2606 "y.tab.c"
break;
case 47:
#line 1209 "2005037.y"
	{logFileWriter("variable","ID");

yyval.parseTreeNode = new ParseTreeNode("variable");
yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

/*let's look for semantic errors*/

SymbolInfo *symbolCurr = symbolTable.lookupCurrentScope(yystack.l_mark[0].parseTreeNode->lexeme);
SymbolInfo * symbolGlobal = symbolTable.lookupGlobalScope(yystack.l_mark[0].parseTreeNode->lexeme);

if(symbolCurr ==nullptr && symbolGlobal ==nullptr){
    string error = "Undeclared variable '"+yystack.l_mark[0].parseTreeNode->lexeme+"'" ;
	errorFileWriter(error,yystack.l_mark[0].parseTreeNode->startLine);
}else{
	SymbolInfo *symbol = symbolCurr==nullptr ? symbolGlobal : symbolCurr ;
	/* cout<<"let's see if I have this data type";*/
	/* cout<<symbol->dataType<<endl ;*/
	yyval.parseTreeNode -> dataType = symbol->dataType ;
}

}
#line 2631 "y.tab.c"
break;
case 48:
#line 1230 "2005037.y"
	{
		logFileWriter("variable","ID LSQUARE expression RSQUARE");
		
		yyval.parseTreeNode = new ParseTreeNode("variable");
		yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		
		
SymbolInfo *symbolCurr = symbolTable.lookupCurrentScope(yystack.l_mark[-3].parseTreeNode->lexeme);
SymbolInfo * symbolGlobal = symbolTable.lookupGlobalScope(yystack.l_mark[-3].parseTreeNode->lexeme);

if(symbolCurr ==nullptr && symbolGlobal ==nullptr){
    string error = "Undeclared variable '"+yystack.l_mark[-3].parseTreeNode->lexeme+"'" ;
	errorFileWriter(error,yystack.l_mark[-3].parseTreeNode->startLine);
	
}else{

SymbolInfo *symbol = symbolCurr == nullptr?symbolGlobal : symbolCurr ;
string error = "'"+yystack.l_mark[-3].parseTreeNode->lexeme+"' is not an array" ;

if(symbol->isFunction){
	
    errorFileWriter(error,yystack.l_mark[-3].parseTreeNode->startLine);
}else{
	IdInfo* idInfo = (IdInfo*)symbol ;
	/*cout<<"symbol is an id"<<endl;*/
	if(idInfo->size==-1){
		errorFileWriter(error,yystack.l_mark[-3].parseTreeNode->startLine);
		/*cout<<"symbol is not an array"<<endl;*/
	}else if(yystack.l_mark[-1].parseTreeNode->dataType=="FLOAT"){
		/*cout<<"symbol is an array with invalid subscript"<<endl;*/
		errorFileWriter("Array subscript is not an integer",yystack.l_mark[-3].parseTreeNode->startLine);
     }else{
		/* I have found no error in array declaration */
		yyval.parseTreeNode -> dataType = idInfo->dataType ;
		/*cout<<"yoo found correct array data type "<<$$->dataType<<endl;*/
	 }
}


}

		}
#line 2680 "y.tab.c"
break;
case 49:
#line 1277 "2005037.y"
	{logFileWriter("expression","logic_expression");
 yyval.parseTreeNode = new ParseTreeNode("expression");
 yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
 yyval.parseTreeNode->dataType = yystack.l_mark[0].parseTreeNode->dataType ;
 }
#line 2689 "y.tab.c"
break;
case 50:
#line 1282 "2005037.y"
	{
		logFileWriter("expression","variable ASSIGNOP logic_expression");
		yyval.parseTreeNode = new ParseTreeNode("expression");
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

		/*hear we come to handle all the errors */
         if(yystack.l_mark[0].parseTreeNode->dataType == "VOID"){
			string error = "Void cannot be used in expression ";
			errorFileWriter(error,yystack.l_mark[0].parseTreeNode->startLine);
		 }else if(yystack.l_mark[-2].parseTreeNode->dataType == "INT" && yystack.l_mark[0].parseTreeNode->dataType=="FLOAT"){
			string error = "Warning: possible loss of data in assignment of FLOAT to INT";
			errorFileWriter(error,yystack.l_mark[-2].parseTreeNode->startLine);
		 }
		

		
		}
#line 2712 "y.tab.c"
break;
case 51:
#line 1302 "2005037.y"
	{
			yyclearin;
			yyval.parseTreeNode = new ParseTreeNode("error","expression",line);
			errorFileWriter("Syntax error at expression of expression statement",line);
			logFile<<"Error at line no "<<line<<" : Syntax error"<<endl;
		}
#line 2722 "y.tab.c"
break;
case 52:
#line 1310 "2005037.y"
	{
	logFileWriter("logic_expression","rel_expression");
    yyval.parseTreeNode = new ParseTreeNode("logic_expression");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	yyval.parseTreeNode->dataType = yystack.l_mark[0].parseTreeNode->dataType ;

}
#line 2733 "y.tab.c"
break;
case 53:
#line 1317 "2005037.y"
	{
			logFileWriter("logic_expression","rel_expression LOGICOP rel_expression");
		     yyval.parseTreeNode = new ParseTreeNode("logic_expression");
			 yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
			 yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
			 yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

			 if(yystack.l_mark[-2].parseTreeNode->dataType == "VOID" || yystack.l_mark[-1].parseTreeNode->dataType == "VOID"){
				yyval.parseTreeNode->dataType = "VOID";
			 }else{
				yyval.parseTreeNode->dataType = "INT";
			 }
		 
		 }
#line 2751 "y.tab.c"
break;
case 54:
#line 1333 "2005037.y"
	{
	logFileWriter("rel_expression","simple_expression");
    yyval.parseTreeNode=new ParseTreeNode("rel_expression");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	yyval.parseTreeNode->dataType = yystack.l_mark[0].parseTreeNode->dataType ;
}
#line 2761 "y.tab.c"
break;
case 55:
#line 1339 "2005037.y"
	{
			logFileWriter("rel_expression","simple_expression RELOP simple_expression");
		    yyval.parseTreeNode=new ParseTreeNode("rel_expression");
			yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
			yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
			yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

			if(yystack.l_mark[-2].parseTreeNode->dataType == "VOID" || yystack.l_mark[0].parseTreeNode->dataType == "VOID"){
				yyval.parseTreeNode->dataType = "VOID";
			}else{
				yyval.parseTreeNode->dataType = "INT";
			}
		}
#line 2778 "y.tab.c"
break;
case 56:
#line 1354 "2005037.y"
	{logFileWriter("simple_expression","term");
yyval.parseTreeNode = new ParseTreeNode("simple_expression");
yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
yyval.parseTreeNode->dataType = yystack.l_mark[0].parseTreeNode->dataType ;

}
#line 2788 "y.tab.c"
break;
case 57:
#line 1360 "2005037.y"
	{
			logFileWriter("simple_expression","simple_expression ADDOP term");
			yyval.parseTreeNode = new ParseTreeNode("simple_expression");
			yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
			yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
			yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

			if(yystack.l_mark[-2].parseTreeNode->dataType == "VOID" || yystack.l_mark[0].parseTreeNode->dataType == "VOID"){
				yyval.parseTreeNode->dataType = "VOID";
			}else if(yystack.l_mark[-2].parseTreeNode->dataType == "FLOAT" || yystack.l_mark[0].parseTreeNode->dataType == "FLOAT"){
				yyval.parseTreeNode->dataType = "FLOAT";
			}else{
				yyval.parseTreeNode->dataType = "INT";
			}
            
			}
#line 2808 "y.tab.c"
break;
case 58:
#line 1378 "2005037.y"
	{
	logFileWriter("term","unary_expression");
	yyval.parseTreeNode = new ParseTreeNode("term");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	yyval.parseTreeNode->dataType = yystack.l_mark[0].parseTreeNode->dataType ;
	
	}
#line 2819 "y.tab.c"
break;
case 59:
#line 1385 "2005037.y"
	{
		logFileWriter("term","term MULOP unary_expression");
		yyval.parseTreeNode = new ParseTreeNode("term");
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
        
		/*cout<<"yoo what is my lexeme "<<$3->lastFoundLexeme<<endl;*/
		string mulop = yystack.l_mark[-1].parseTreeNode->lexeme ;

		if(yystack.l_mark[-2].parseTreeNode->dataType == "VOID" || yystack.l_mark[0].parseTreeNode->dataType == "VOID"){
			/* cannot use arithmatic operator on void*/
			yyval.parseTreeNode->dataType = "VOID";
		}else if(yystack.l_mark[0].parseTreeNode->lastFoundLexeme=="0" && (mulop == "%" || mulop == "/") ){
			string error = "Warning: division by zero i=0f=1Const=0" ;
			errorFileWriter(error,yystack.l_mark[0].parseTreeNode->startLine);
			if(mulop=="%"){
				yyval.parseTreeNode->dataType = "INT";
			}else{
				yyval.parseTreeNode->dataType = yystack.l_mark[-2].parseTreeNode->dataType ;
			}
		}else if(mulop=="%" && (yystack.l_mark[-2].parseTreeNode->dataType == "FLOAT" || yystack.l_mark[0].parseTreeNode->dataType == "FLOAT")){
			string error = "Operands of modulus must be integers ";
			errorFileWriter(error,yystack.l_mark[-2].parseTreeNode->startLine);
			yyval.parseTreeNode->dataType = "INT";
		}else if(yystack.l_mark[-2].parseTreeNode->dataType == "FLOAT" || yystack.l_mark[0].parseTreeNode->dataType == "FLOAT"){
			yyval.parseTreeNode->dataType = "FLOAT";
		}else{
			yyval.parseTreeNode->dataType = yystack.l_mark[-2].parseTreeNode->dataType ;
		}

		
		}
#line 2856 "y.tab.c"
break;
case 60:
#line 1420 "2005037.y"
	{
	logFileWriter("unary_expression","ADDOP unary_expression");
	yyval.parseTreeNode = new ParseTreeNode("unary_expression");
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	yyval.parseTreeNode->dataType = yystack.l_mark[0].parseTreeNode->dataType ;

	}
#line 2868 "y.tab.c"
break;
case 61:
#line 1428 "2005037.y"
	{
			logFileWriter("unary_expression","NOT unary_expression");
			yyval.parseTreeNode = new ParseTreeNode("unary_expression");
			yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
			yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
            yyval.parseTreeNode->dataType = yystack.l_mark[0].parseTreeNode->dataType ;

			/* if($2->dataType == "VOID"){*/
			/* 	string error = "Cannot use unary operator on void type";*/
			/* 	errorFileWriter(error,$2->startLine);*/
			/* }*/
			}
#line 2884 "y.tab.c"
break;
case 62:
#line 1440 "2005037.y"
	{
			logFileWriter("unary_expression","factor");
			yyval.parseTreeNode = new ParseTreeNode("unary_expression");
			yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
            yyval.parseTreeNode->dataType = yystack.l_mark[0].parseTreeNode->dataType ;
			}
#line 2894 "y.tab.c"
break;
case 63:
#line 1448 "2005037.y"
	{
	logFileWriter("factor","variable");
	yyval.parseTreeNode = new ParseTreeNode("factor");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	yyval.parseTreeNode->dataType = yystack.l_mark[0].parseTreeNode->dataType ;
	
	}
#line 2905 "y.tab.c"
break;
case 64:
#line 1455 "2005037.y"
	{
		logFileWriter("factor","ID LPAREN argument_list RPAREN");
		yyval.parseTreeNode = new ParseTreeNode("factor");
		/*cout<<"matching the rule"<<endl;*/
		yyval.parseTreeNode->addChild(yystack.l_mark[-3].parseTreeNode);
		/*cout<<"child 1 added"<<endl;*/
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		/*cout<<"child 2 added"<<endl;*/
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		/*cout<<"child 3 added"<<endl;*/
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		/*cout<<"child 4 added"<<endl;*/

		SymbolInfo* symbol = symbolTable.lookup(yystack.l_mark[-3].parseTreeNode->lexeme);

		if(symbol == nullptr || (!symbol->isFunction) ){
			string error = "Undeclared function '"+yystack.l_mark[-3].parseTreeNode->lexeme+"'";
			errorFileWriter(error,yystack.l_mark[-3].parseTreeNode->startLine);
		}else{
			/*our symbol is a function */
			FunctionInfo * func = (FunctionInfo*)symbol ;
			/*but what are its arguments!!! already stored in argList ^_^*/

			/*cout<<"my size"<<argList.size()<<endl;*/

			/*let's check for wrong number of arguments*/
			int paramCount = func->getNumberOfParameters();
			int argCount = argList.size();
			bool mismatch = false;
            string error ;
			if(argCount<paramCount){
				mismatch = true;
                error = "Too few arguments to function '"+yystack.l_mark[-3].parseTreeNode->lexeme+"'";
				errorFileWriter(error,yystack.l_mark[-3].parseTreeNode->startLine);
			}else if(argCount>paramCount){
                mismatch = true;
                error = "Too many arguments to function '"+yystack.l_mark[-3].parseTreeNode->lexeme+"'";
				errorFileWriter(error,yystack.l_mark[-3].parseTreeNode->startLine);
			}else{
				/*let's see the paramList now ;*/
                int i=0 ;

				for(const auto &args : argList){

					if(args != func->findParamAtIndex(i)){
						bool mismatch = true ;
						error = "Type mismatch for argument "+to_string(i+1)+ " of '"+yystack.l_mark[-3].parseTreeNode->lexeme+"'";
						errorFileWriter(error,yystack.l_mark[-3].parseTreeNode->startLine);
					}
					i++ ;
				}
			}
			if(!mismatch){
				yyval.parseTreeNode->dataType = func->getReturnType() ;
			}
		}

		argList.clear();
		
		}
#line 2969 "y.tab.c"
break;
case 65:
#line 1515 "2005037.y"
	{
		logFileWriter("factor","LPAREN expression RPAREN");
		yyval.parseTreeNode = new ParseTreeNode("factor");
		yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		yyval.parseTreeNode->dataType = yystack.l_mark[-1].parseTreeNode->dataType;
		}
#line 2981 "y.tab.c"
break;
case 66:
#line 1523 "2005037.y"
	{
		logFileWriter("factor","CONST_INT");
		yyval.parseTreeNode = new ParseTreeNode("factor");
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		yyval.parseTreeNode->dataType = "INT";
		
		}
#line 2992 "y.tab.c"
break;
case 67:
#line 1530 "2005037.y"
	{
		logFileWriter("factor","CONST_FLOAT");
		yyval.parseTreeNode = new ParseTreeNode("factor");
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		yyval.parseTreeNode->dataType = "FLOAT";
		}
#line 3002 "y.tab.c"
break;
case 68:
#line 1536 "2005037.y"
	{
		logFileWriter("factor","variable INCOP");
		yyval.parseTreeNode = new ParseTreeNode("factor");
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		yyval.parseTreeNode->dataType = yystack.l_mark[-1].parseTreeNode->dataType ;
		}
#line 3013 "y.tab.c"
break;
case 69:
#line 1543 "2005037.y"
	{
		logFileWriter("factor","variable DECOP");
		yyval.parseTreeNode = new ParseTreeNode("factor");
		yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		yyval.parseTreeNode->dataType = yystack.l_mark[-1].parseTreeNode->dataType ;
		}
#line 3024 "y.tab.c"
break;
case 70:
#line 1552 "2005037.y"
	{
	logFileWriter("argument_list","arguments");
	yyval.parseTreeNode = new ParseTreeNode("argument_list");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

	}
#line 3034 "y.tab.c"
break;
case 71:
#line 1558 "2005037.y"
	{
				/*argument list can be empty but argument list is a valid node*/
				yyval.parseTreeNode = new ParseTreeNode("argument_list");
			  }
#line 3042 "y.tab.c"
break;
case 72:
#line 1564 "2005037.y"
	{
	logFileWriter("arguments","arguments COMMA logic_expression");
	yyval.parseTreeNode = new ParseTreeNode("arguments");
	yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	argList.push_back(yystack.l_mark[0].parseTreeNode->dataType);
}
#line 3054 "y.tab.c"
break;
case 73:
#line 1572 "2005037.y"
	{
			logFileWriter("arguments","logic_expression");
			yyval.parseTreeNode = new ParseTreeNode("arguments");
			yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
			/*declare an argument list and push the arguments here*/
			argList.push_back(yystack.l_mark[0].parseTreeNode->dataType);
		  }
#line 3065 "y.tab.c"
break;
#line 3067 "y.tab.c"
    default:
        break;
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark -= yym;
#endif
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
        {
            fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
            if (!yytrial)
#endif /* YYBTYACC */
                fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[YYFINAL], yyval));
#endif
            fprintf(stderr, "shifting from state 0 to final state %d\n", YYFINAL);
        }
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        *++yystack.p_mark = yyloc;
#endif
        if (yychar < 0)
        {
#if YYBTYACC
            do {
            if (yylvp < yylve)
            {
                /* we're currently re-reading tokens */
                yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylloc = *yylpp++;
#endif
                yychar = *yylexp++;
                break;
            }
            if (yyps->save)
            {
                /* in trial mode; save scanner results for future parse attempts */
                if (yylvp == yylvlim)
                {   /* Enlarge lexical value queue */
                    size_t p = (size_t) (yylvp - yylvals);
                    size_t s = (size_t) (yylvlim - yylvals);

                    s += YYLVQUEUEGROWTH;
                    if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL)
                        goto yyenomem;
                    if ((yylvals   = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL)
                        goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    if ((yylpsns   = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL)
                        goto yyenomem;
#endif
                    yylvp   = yylve = yylvals + p;
                    yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpp   = yylpe = yylpsns + p;
                    yylplim = yylpsns + s;
#endif
                    yylexp  = yylexemes + p;
                }
                *yylexp = (YYINT) YYLEX;
                *yylvp++ = yylval;
                yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                *yylpp++ = yylloc;
                yylpe++;
#endif
                yychar = *yylexp++;
                break;
            }
            /* normal operation, no conflict encountered */
#endif /* YYBTYACC */
            yychar = YYLEX;
#if YYBTYACC
            } while (0);
#endif /* YYBTYACC */
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)\n",
                                YYDEBUGSTR, yydepth, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
    {
        fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
            fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[yystate], yyval));
#endif
        fprintf(stderr, "shifting from state %d to state %d\n", *yystack.s_mark, yystate);
    }
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    *++yystack.p_mark = yyloc;
#endif
    goto yyloop;
#if YYBTYACC

    /* Reduction declares that this path is valid. Set yypath and do a full parse */
yyvalid:
    if (yypath) YYABORT;
    while (yyps->save)
    {
        YYParseState *save = yyps->save;
        yyps->save = save->save;
        save->save = yypath;
        yypath = save;
    }
#if YYDEBUG
    if (yydebug)
        fprintf(stderr, "%s[%d]: state %d, CONFLICT trial successful, backtracking to state %d, %d tokens\n",
                        YYDEBUGSTR, yydepth, yystate, yypath->state, (int)(yylvp - yylvals - yypath->lexeme));
#endif
    if (yyerrctx)
    {
        yyFreeState(yyerrctx);
        yyerrctx = NULL;
    }
    yylvp          = yylvals + yypath->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yylpp          = yylpsns + yypath->lexeme;
#endif
    yylexp         = yylexemes + yypath->lexeme;
    yychar         = YYEMPTY;
    yystack.s_mark = yystack.s_base + (yypath->yystack.s_mark - yypath->yystack.s_base);
    memcpy (yystack.s_base, yypath->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
    yystack.l_mark = yystack.l_base + (yypath->yystack.l_mark - yypath->yystack.l_base);
    memcpy (yystack.l_base, yypath->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark = yystack.p_base + (yypath->yystack.p_mark - yypath->yystack.p_base);
    memcpy (yystack.p_base, yypath->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
    yystate        = yypath->state;
    goto yyloop;
#endif /* YYBTYACC */

yyoverflow:
    YYERROR_CALL("yacc stack overflow");
#if YYBTYACC
    goto yyabort_nomem;
yyenomem:
    YYERROR_CALL("memory exhausted");
yyabort_nomem:
#endif /* YYBTYACC */
    yyresult = 2;
    goto yyreturn;

yyabort:
    yyresult = 1;
    goto yyreturn;

yyaccept:
#if YYBTYACC
    if (yyps->save) goto yyvalid;
#endif /* YYBTYACC */
    yyresult = 0;

yyreturn:
#if defined(YYDESTRUCT_CALL)
    if (yychar != YYEOF && yychar != YYEMPTY)
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval, &yylloc);
#else
        YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */

    {
        YYSTYPE *pv;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        YYLTYPE *pp;

        for (pv = yystack.l_base, pp = yystack.p_base; pv <= yystack.l_mark; ++pv, ++pp)
             YYDESTRUCT_CALL("cleanup: discarding state",
                             yystos[*(yystack.s_base + (pv - yystack.l_base))], pv, pp);
#else
        for (pv = yystack.l_base; pv <= yystack.l_mark; ++pv)
             YYDESTRUCT_CALL("cleanup: discarding state",
                             yystos[*(yystack.s_base + (pv - yystack.l_base))], pv);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
    }
#endif /* defined(YYDESTRUCT_CALL) */

#if YYBTYACC
    if (yyerrctx)
    {
        yyFreeState(yyerrctx);
        yyerrctx = NULL;
    }
    while (yyps)
    {
        YYParseState *save = yyps;
        yyps = save->save;
        save->save = NULL;
        yyFreeState(save);
    }
    while (yypath)
    {
        YYParseState *save = yypath;
        yypath = save->save;
        save->save = NULL;
        yyFreeState(save);
    }
#endif /* YYBTYACC */
    yyfreestack(&yystack);
    return (yyresult);
}
