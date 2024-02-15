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

//necessary global variables 
string varType ;  // stores recent variable type
string funcName,funcReturnType ;
list<pair<string,string>> parameters ; //Contains the parameter list <name, type> of the currently declared function
list<string> argList ;  //contains the argument list while invoking a function

bool definingFunction = false ;
//to know the state of the function
bool isError = false ; //to know if there has been syntax error in function call
bool hasDeclListError = false ;

// ICG starts here  // 

int offset = 0; //to track location of local variables into the stack
bool hasPrint = false;
Register reg ;   // works as a register manager
vector<pair<string,int>>params ; //to store func arguments and their offset
int paramOffset ;
int returnLabel ;
string returnAddr;

void yyerror(char *s)
{
	//write your code
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

// let's gen data segment

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

	int offset = info->offset ;

	string base = "BP-" ;

	if(offset<0){
		offset = -offset ;
		base = "BP+" ;
	}

	return "["+base+to_string(offset)+"]";
}

void writeLabel(int label){
	assemblyFile<<"L"<<label<<":\n";
}

string formatLabel(int label){
	return "L"+to_string(label);
}

string mov(const string &dest,const string &src){
	return "\tMOV "+dest+","+src+"\n";
}

string add(const string &dest,const string &src){
	return "\tADD "+dest+","+src+"\n";
}
string sub(const string &dest,const string &src){
	return "\tSUB "+dest+","+src+"\n";
}

string cmp(const string&dest,const string&src){
	return "\tCMP "+dest+","+src+"\n";
}

string jump(const string&jmp,int label){
	return "\t"+jmp+" L"+to_string(label)+"\n";
}

string push(const string&reg){
	return "\tPUSH "+reg+"\n";
}

void traverseAndGenerate(ParseTreeNode*root){

	if(root->isLeaf){
		//decision about leaf nodes have already been taken
		return ;
	}

	string rule = root->name+" :"+root->nameList ;

	if(rule=="arguments : arguments COMMA logic_expression"){

		traverseAndGenerate(root->children[0]);
		string dest = reg.getRegister();

		traverseAndGenerate(root->children[2]);
		
		assemblyFile<<mov(dest,root->children[2]->addr);
		reg.resetRegister(root->children[2]->addr);

		assemblyFile<<push(dest);

		reg.resetRegister(dest);

		return;
	}

	if(rule=="arguments : logic_expression"){
		traverseAndGenerate(root->children[0]);
		string dest = reg.getRegister();

		assemblyFile<<mov(dest,root->children[0]->addr);
		reg.resetRegister(root->children[0]->addr);

		assemblyFile<<push(dest);

		reg.resetRegister(dest);

		return;
	}

	if(rule=="statement : RETURN expression SEMICOLON"){
		
		traverseAndGenerate(root->children[1]);
		string dest = reg.getRegister();

		if(dest!="CX"){
			reg.resetRegister(dest);
			dest = "CX" ;
		}

		assemblyFile<<mov(dest,root->children[1]->addr);
		reg.resetRegister(root->children[1]->addr);

		assemblyFile<<jump("jmp",returnLabel);
		returnAddr = dest ;

		return;
	}

	if(rule=="parameter_list : parameter_list COMMA type_specifier ID"){
		
		string lexeme = root->children[3]->lexeme ;
		paramOffset -= 2 ;
		params.push_back(make_pair(lexeme,paramOffset));
		traverseAndGenerate(root->children[0]);
		return;
	}

	if(rule=="parameter_list : type_specifier ID"){
		string lexeme = root->children[1]->lexeme ;
		paramOffset -= 2 ;
		params.push_back(make_pair(lexeme,paramOffset));
		return;
	}

	if(rule=="func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement"){
		cout<<"gotcha\n";

		//let us generate dummy func code

		string func_name = root->children[1]->lexeme ;
		assemblyFile<<func_name<<" PROC\n";
		assemblyFile<<"\tPUSH BP\n";
		assemblyFile<<"\tMOV BP,SP\n";
		
		params.clear();
		paramOffset = -2 ;
		offset = 0 ;
		returnLabel = 0 ;
		returnAddr = "" ;

		returnLabel = ParseTreeNode::getLabel();

		traverseAndGenerate(root->children[3]);

		traverseAndGenerate(root->children[5]);
		
		int argCount = - (paramOffset + 2) ;

		writeLabel(returnLabel);

		assemblyFile<<add("SP",to_string(offset));
		assemblyFile<<"\tPOP BP\n";
		assemblyFile<<"\tRET "<<argCount<<"\n";

		if(returnAddr!=""){
			SymbolInfo *info = symbolTable.lookup(func_name);
			info->returnAddr = returnAddr ;
			reg.resetRegister(returnAddr);
			cout<<"resetted "<<returnAddr<<"\n";
		}

		params.clear();

		return;
	}

	if(rule=="factor : ID LPAREN argument_list RPAREN"){
		cout<<"func found"<<endl;

		traverseAndGenerate(root->children[2]);

		string func_name = root->children[0]->lexeme ;

		assemblyFile<<"\tCALL "<<func_name<<endl;

		SymbolInfo *info = symbolTable.lookup(func_name);

		root->addr = info->returnAddr ;

		cout<<root->addr<<" got func reg\n";

		return ;
	}

	if(rule=="statement : WHILE LPAREN expression RPAREN statement"){
		cout<<"while rule matched\n";
		int startLabel = ParseTreeNode::getLabel();
		writeLabel(startLabel);

		traverseAndGenerate(root->children[2]);

		int trueLabel = ParseTreeNode::getLabel();
		int falseLabel = ParseTreeNode::getLabel();

		assemblyFile<<cmp(root->children[2]->addr,"1");
		reg.resetRegister(root->children[2]->addr);

		assemblyFile<<jump("JGE",trueLabel);
		assemblyFile<<jump("jmp",falseLabel);

		writeLabel(trueLabel);

		traverseAndGenerate(root->children[4]);
		assemblyFile<<jump("jmp",startLabel);

		writeLabel(falseLabel);

		return;
	}

	if(rule=="statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement"){
		
		traverseAndGenerate(root->children[2]);
		int startLabel = ParseTreeNode::getLabel();

		writeLabel(startLabel);

		traverseAndGenerate(root->children[3]);

		string conditionAddr = root->children[3]->addr ;

		assemblyFile<<cmp(conditionAddr,"1");

		reg.resetRegister(conditionAddr);

		int trueLabel = ParseTreeNode::getLabel();
		int falseLabel = ParseTreeNode::getLabel();

		assemblyFile<<jump("JE",trueLabel);
		assemblyFile<<jump("jmp",falseLabel);

		writeLabel(trueLabel);

		traverseAndGenerate(root->children[6]);
		traverseAndGenerate(root->children[4]);
		assemblyFile<<jump("jmp",startLabel);

		writeLabel(falseLabel);

		reg.resetRegister(root->children[6]->addr,root->children[4]->addr);

		return;
	}

	if(rule=="statement : IF LPAREN expression RPAREN statement ELSE statement"){
		
		traverseAndGenerate(root->children[2]);
		string exp_result = reg.getRegister() ;
		assemblyFile<<mov(exp_result,root->children[2]->addr);
		reg.resetRegister(root->children[2]->addr);

		int trueLabel = ParseTreeNode::getLabel();
		int falseLabel = ParseTreeNode::getLabel();
		int finalLabel = ParseTreeNode::getLabel();
		
		assemblyFile<<cmp(exp_result,"1");
		reg.resetRegister(exp_result);


		assemblyFile<<jump("JE",trueLabel);
		assemblyFile<<jump("jmp",falseLabel);

		writeLabel(trueLabel);
		traverseAndGenerate(root->children[4]);
		assemblyFile<<jump("jmp",finalLabel);

		writeLabel(falseLabel);

		traverseAndGenerate(root->children[6]);

		writeLabel(finalLabel);

		reg.resetRegister(root->children[4]->addr);
		
		return ;
	}

	if(rule=="statement : IF LPAREN expression RPAREN statement"){
		
		traverseAndGenerate(root->children[2]);
		string exp_result = reg.getRegister() ;
		assemblyFile<<mov(exp_result,root->children[2]->addr);
		reg.resetRegister(root->children[2]->addr);

		int trueLabel = ParseTreeNode::getLabel();
		int finalLabel = ParseTreeNode::getLabel();
		
		assemblyFile<<cmp(exp_result,"1");

		reg.resetRegister(exp_result);

		assemblyFile<<jump("JE",trueLabel);
		assemblyFile<<jump("jmp",finalLabel);

		writeLabel(trueLabel);
		traverseAndGenerate(root->children[4]);

		writeLabel(finalLabel);
		root->label = finalLabel;

		reg.resetRegister(root->children[4]->addr);
		
		return ;
	}

	if(rule=="logic_expression : rel_expression LOGICOP rel_expression"){

		traverseAndGenerate(root->children[0]);

		string l = reg.getRegister();
        string r;
		assemblyFile<<mov(l,root->children[0]->addr);

		reg.resetRegister(root->children[0]->addr);

		string lexeme = root->children[1]->lexeme ;

		int interLabel = ParseTreeNode::getLabel();
		int trueLabel = ParseTreeNode::getLabel();
		int falseLabel = ParseTreeNode::getLabel();
		int finalLabel = ParseTreeNode::getLabel();

		assemblyFile<<cmp(l,"0");

		reg.resetRegister(l);

		if(lexeme=="&&"){
			cout<<"code for and"<<endl;

			assemblyFile<<jump("JNE",interLabel);
			assemblyFile<<jump("JMP",falseLabel);		

		}else if(lexeme=="||"){
			assemblyFile<<jump("JNE",trueLabel);
			assemblyFile<<jump("JMP",interLabel);
		}

		    writeLabel(interLabel);

		   traverseAndGenerate(root->children[2]);

		   r = reg.getRegister();

		   assemblyFile<<mov(r,root->children[2]->addr);

		   reg.resetRegister(root->children[2]->addr);


			string code = cmp(r,"0");
			code += jump("JNE",trueLabel);
			code += jump("JMP",falseLabel);

			assemblyFile<<code ;

		    writeLabel(trueLabel);
		    reg.resetRegister(l,r);

		string ans = reg.getRegister();

		assemblyFile<<mov(ans,"1");
		assemblyFile<<jump("JMP",finalLabel);
		writeLabel(falseLabel);

		assemblyFile<<mov(ans,"0");

		writeLabel(finalLabel);

		root->addr = ans ;

		return;
	}

	if(rule=="unary_expression : ADDOP unary_expression"){
		
		traverseAndGenerate(root->children[1]);
		
		string lexeme = root->children[0]->lexeme;

		string dest = reg.getRegister();

		assemblyFile<<mov(dest,root->children[1]->addr);

		reg.resetRegister(root->children[1]->addr);

		if(lexeme=="-"){
			assemblyFile<<"\tNEG "+dest+"\n";
		}

		root->addr = dest;
		
		return;
	}

	if(rule=="factor : variable INCOP" || rule=="factor : variable DECOP"){
		
		traverseAndGenerate(root->children[0]);

		string dest = reg.getRegister();
		string lexeme = root->children[1]->lexeme ;
		string operation = "ADD";
		string reverse = "SUB" ;

		if(lexeme == "--"){
			operation = "SUB";
			reverse = "ADD" ;
		}

		string code;

		code+=mov(dest,root->children[0]->addr);
		code+="\t"+operation+" "+dest+",1\n";
		code+=mov(root->children[0]->addr,dest);
		code += "\t" + reverse + " "+dest+",1\n";
		assemblyFile<<code ;
		root->addr = dest;
		return ;
	}

	if(rule=="rel_expression : simple_expression RELOP simple_expression"){
		
		traverseAndGenerate(root->children[0]);
		assemblyFile<<push(root->children[0]->addr);
		reg.resetRegister(root->children[0]->addr);

		traverseAndGenerate(root->children[2]);

		string l = reg.getRegister() ;
		assemblyFile<<"\tPOP "<<l<<endl;

		string r = root->children[2]->addr ;

		string operation ;

		string lexeme = root->children[1]->lexeme ;

		if(lexeme=="<") operation = "JL" ;
		else if(lexeme=="<=") operation = "JLE" ;
		else if(lexeme==">") operation = "JG" ;
		else if(lexeme==">=") operation = "JGE" ;
		else if(lexeme=="==") operation = "JE" ;
		else if(lexeme=="!=") operation = "JNE" ;

		cout<<"operation selected "<<operation<<endl;

		
		string code = cmp(l,r) ;

		root->label = (ParseTreeNode::getLabel()) ;

		code +=  jump(operation,root->label);

		int falseLabel = ParseTreeNode::getLabel() ;

		code += "\tjmp "+formatLabel(falseLabel) + "\n";

		assemblyFile<<code ;

		writeLabel(root->label);
		string dest = reg.getRegister();

		code = "\tMOV "+dest+",1\n";

		int finalLabel = ParseTreeNode::getLabel();

		code+= "\tjmp "+formatLabel(finalLabel) + "\n";

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
	
		    string multiplicand, multiplier  ;

		    traverseAndGenerate(root->children[0]);

            assemblyFile<<push(root->children[0]->addr) ;
			reg.resetRegister(root->children[0]->addr) ;

		    traverseAndGenerate(root->children[2]);
            assemblyFile<<push(root->children[2]->addr) ;
            reg.resetRegister(root->children[2]->addr) ;

		    multiplicand = reg.getRegister();
			multiplier = reg.getRegister();

			cout<<"first time yoo"<<multiplier<<" "<<multiplicand<<" "<<endl;
			assemblyFile<<"\tPOP "+multiplier+"\n" ;
			assemblyFile<<"\tPOP "+multiplicand+"\n" ;

			//some guardian clauses

		   if(multiplier=="AX"){
			multiplier = reg.getRegister();
			assemblyFile<<mov(multiplier,"AX");
		   }

		
	    string code ;

		if(multiplicand != "AX"){
			code += mov("AX",multiplicand);
			reg.resetRegister(multiplicand);
			reg.acquireRegister("AX");
		}

		cout<<"after check: multiplier "<<multiplier<<"multiplicand "<<multiplicand<<endl;

		string operation ;

		if(root->children[1]->lexeme=="*"){
			operation= "MUL" ;
		}else{
			operation = "DIV" ;
		}


		code += "\t"+operation+" "+multiplier+"\n" ;

		cout<<code<<endl;

		reg.resetRegister(multiplier);

		assemblyFile<<code ;

		if(root->children[1]->lexeme=="%"){
			root->addr = "DX" ;
			reg.acquireRegister("DX");
			reg.resetRegister("AX");
			}
		else
			root->addr = "AX" ;


		return;
	}

	if(rule=="simple_expression : simple_expression ADDOP term"){

		traverseAndGenerate(root->children[0]);

		assemblyFile<<push(root->children[0]->addr);
		reg.resetRegister(root->children[0]->addr);
		
		traverseAndGenerate(root->children[2]);

		string dest = reg.getRegister();
		assemblyFile<<"\tPOP "+dest<<endl;

		string src = root->children[2]->addr;
		
		
		string operation = "ADD" ;
		string lexeme = root->children[1]->lexeme ;

		if(lexeme=="-"){
			operation = "SUB" ;
		}

		string code = "\t"+operation+" "+dest+","+src+"\n";
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

	if(rule=="unary_expression : factor" || rule=="term : unary_expression" || rule=="simple_expression : term"||rule=="rel_expression : simple_expression"||rule=="logic_expression : rel_expression" || rule=="factor : variable" || rule=="expression : logic_expression"){
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

		//now we can reset the source,dest is the new source
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

		//insert func_params here

		for(auto it=params.begin(); it!=params.end(); it++){
			symbolTable.insert(it->first,"ID","INT",-1,symbolTable.isCurrentScopeGlobal(),it->second);
		}

		traverseAndGenerate(root->children[1]);	

		symbolTable.exitScope();

		//cout<<symbolTable.printAll()<<endl;

		//reset the register
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
		returnAddr="" ;

		string code = funcName+" PROC"+"\n" ;

			if(funcName == "main"){
				cout<<"symbol is main function\n";
				code += "\tMOV AX,@DATA\n" ;
				code += "\tMOV DS,AX\n" ;
			}

			code += "\tPUSH BP\n" ;
			code += "\tMOV BP,SP\n" ;

		assemblyFile<<code ;

		returnLabel = ParseTreeNode::getLabel();

		traverseAndGenerate(root->children[4]);

		writeLabel(returnLabel);

		//storing the return address
		SymbolInfo* symbol = symbolTable.lookup(root->children[1]->lexeme);

		symbol->returnAddr = root->children[4]->addr ;

		//putting SP add amount here//
		assemblyFile<<"\tADD SP,"<<offset<<"\n" ;
		assemblyFile<<"\tPOP BP\n";
		if(root->children[1]->lexeme=="main"){
			//cout<<"main func ended"<<endl;
			assemblyFile<<"\tMOV AX,4CH\n";
			assemblyFile<<"\tINT 21H\n" ;
			
		}else{
			assemblyFile<<"\tRET\n" ;
		}

		assemblyFile<<root->children[1]->lexeme<<" ENDP\n";

		if(returnAddr!=""){
			SymbolInfo *info = symbolTable.lookup(funcName);
			info->returnAddr = returnAddr ;
			reg.resetRegister(returnAddr);
			cout<<"resetted "<<returnAddr<<"\n";
		}

		return ;

	}


	for(ParseTreeNode*node : root->children){
		traverseAndGenerate(node);
	}


}

void genEndingCode(){

	if(hasPrint){
		// code for including print function
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

//here comes the beast who prints the entire parseTree

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
	//cout<<symbolTable.printAll()<<endl;
	logFile<<symbolTable.printAll();
}


/* necessary functions for recognizing semantic errors */


void handleIdDeclaration(ParseTreeNode* node,int size){

	if(hasDeclListError){
		hasDeclListError = false;
		return ;
	}

	//extracting information 

	string lexeme = node->lexeme ;
	string token = node->token;
	string idType = toUpper(varType) ;
	int lineCount = node->startLine;

	//looking for error 

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
    // no error in declaration, insert in symbolTable
	symbolTable.insert(lexeme,token,idType,size,symbolTable.isCurrentScopeGlobal(),offset);
	//cout<<lexeme<<" inserted in table "<<endl;
	//cout<<symbolTable.printAll()<<endl;

}

void handleFunctionDeclaration(const string&name,const string&returnType,int lineCount){
	funcName = name ;
	funcReturnType = returnType ;
	FunctionInfo *functionInfo ;
    //cout<<"function called\n";
	if(symbolTable.insert(name,"ID",true)){
	  //insertion successful
	  //let's print the symbolTable 
	  //cout<<symbolTable.printAll();

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
			//this function was declared or defined previously
			//cout<<"this function was declared or defined previously\n";

			functionInfo = (FunctionInfo*)symbol ;
			//cout<<"new funcs return type "<<returnType<<endl ;

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
%token <parseTreeNode> NOT LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON COLON INCOP DECOP ASSIGNOP 
%token <parseTreeNode> CONST_INT CONST_FLOAT LOGICOP RELOP ADDOP BITOP MULOP ID

%type <parseTreeNode> start program unit var_declaration func_declaration func_definition type_specifier parameter_list compound_statement statements declaration_list statement expression_statement expression variable logic_expression rel_expression simple_expression term unary_expression factor argument_list arguments switch_declaration switch_body unit_body default_body

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
		codeGenerator($$);
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
	if(!isError){
		handleFunctionDeclaration(funcName,toUpper(funcReturnType),discoveryLine);
    }else{
		isError = false ;
	}
	
	
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
		| error {
			// attempting to catch syntax error 
			yyclearin;
			isError = true;
			$$ = new ParseTreeNode("error","parameter_list",line);
			errorFileWriter("Syntax error at parameter list of function definition",line);
			logFile<<"Error at line no "<<line<<" : Syntax error"<<endl;

		}
 		;

 		
compound_statement : LCURL statements RCURL {
	logFileWriter("compound_statement","LCURL statements RCURL");
    $$ = new ParseTreeNode("compound_statement");
	$$->addChild($1);
	$$->addChild($2);
	$$->addChild($3);
	$$->offset = $2->offset ;
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
$$->offset = offset ;

}
 		 ;
 		 
type_specifier	: INT  {
							logFileWriter("type_specifier","INT");
							varType = "INT" ;
							$$ = new ParseTreeNode("type_specifier");
							$$->addChild($1);


					   }
 		| FLOAT        {
							logFileWriter("type_specifier", "FLOAT");
							varType = "FLOAT" ;
							$$ = new ParseTreeNode("type_specifier");
							$$->addChild($1);
					   }
 		| VOID		  {
							logFileWriter("type_specifier", "VOID");
							varType = "VOID" ;
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
			//cout<<"found array size "<<size<<endl;
            handleIdDeclaration($3,size);
			
		  }
 		  | ID {
			logFileWriter("declaration_list","ID");
			handleIdDeclaration($1,-1); // -1 indicating that it's not an array
			cout<<offset<<endl;
			$$ = new ParseTreeNode("declaration_list");
			$$->addChild($1);
			
			}
 		  | ID LTHIRD CONST_INT RTHIRD  {
			logFileWriter("declaration_list","ID LSQUARE CONST_INT RSQUARE");
			$$ = new ParseTreeNode("declaration_list");
			$$->addChild($1);
			$$->addChild($2);
			$$->addChild($3);
			$$->addChild($4);

			int size = stoi($3->lexeme) ;
			//cout<<"found array size "<<size<<endl;
            handleIdDeclaration($1,size);
		  }

		  | error {
			yyclearin;
			hasDeclListError = true;
			$$ = new ParseTreeNode("error","declaration_list",line);
			errorFileWriter("Syntax error at declaration list of variable declaration",line);
			logFile<<"Error at line no "<<line<<" : Syntax error"<<endl;
		  }
 		  ;
 		  
statements : statement {logFileWriter("statements","statement");
    $$ = new ParseTreeNode("statements");
	$$->addChild($1);
	$$->offset = $1->offset ;
}
	   | statements statement {
		logFileWriter("statements","statements statement");
	
	    $$ = new ParseTreeNode("statements");
		$$->addChild($1);   
		$$->addChild($2);   
		$$->offset = $2->offset ;
	   }
	   ;
	   
statement : var_declaration {
	logFileWriter("statement","var_declaration");
    $$ = new ParseTreeNode("statement");
    $$->addChild($1);
	$$->offset = $1->offset ;
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
	  | switch_declaration {
				
				$$ = new ParseTreeNode("statement");
				$$->addChild($1);
			}
 		    ;

switch_declaration : SWITCH LPAREN expression RPAREN LCURL switch_body RCURL {

	$$ = new ParseTreeNode("switch_declaration");
	$$->addChild($1);
	$$->addChild($2);
	$$->addChild($3);
	$$->addChild($4);
	$$->addChild($5);
	$$->addChild($6);
	$$->addChild($7);


}
;

switch_body : unit_body default_body {

	$$ = new ParseTreeNode("switch_body");
	$$->addChild($1);
	$$->addChild($2);

                 }
				 | unit_body {
					$$ = new ParseTreeNode("switch_body");
					$$->addChild($1);
				 }
				 ;

unit_body : unit_body CASE CONST_INT COLON statements BREAK SEMICOLON {

	$$ = new ParseTreeNode("unit_body");
	$$->addChild($1);
	$$->addChild($2);
	$$->addChild($3);
	$$->addChild($4);
	$$->addChild($5);
	$$->addChild($6);
	$$->addChild($7);

}
          | CASE CONST_INT COLON statements BREAK SEMICOLON {

			$$ = new ParseTreeNode("unit_body");
			$$->addChild($1);
	        $$->addChild($2);
	        $$->addChild($3);
	        $$->addChild($4);
	        $$->addChild($5);
	        $$->addChild($6);


		  }
		  ;

default_body : DEFAULT COLON statements BREAK SEMICOLON {

	$$ = new ParseTreeNode("default_body");
    $$->addChild($1);
	$$->addChild($2);
	$$->addChild($3);
	$$->addChild($4);
	$$->addChild($5);

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
		logFileWriter("variable","ID LSQUARE expression RSQUARE");
		
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
	}else if($3->dataType=="FLOAT"){
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
 $$->dataType = $1->dataType ;
 }
	   | variable ASSIGNOP logic_expression {
		logFileWriter("expression","variable ASSIGNOP logic_expression");
		$$ = new ParseTreeNode("expression");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);

		//hear we come to handle all the errors 
         if($3->dataType == "VOID"){
			string error = "Void cannot be used in expression ";
			errorFileWriter(error,$3->startLine);
		 }else if($1->dataType == "INT" && $3->dataType=="FLOAT"){
			string error = "Warning: possible loss of data in assignment of FLOAT to INT";
			errorFileWriter(error,$1->startLine);
		 }
		

		
		}

		| error {
			yyclearin;
			$$ = new ParseTreeNode("error","expression",line);
			errorFileWriter("Syntax error at expression of expression statement",line);
			logFile<<"Error at line no "<<line<<" : Syntax error"<<endl;
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
			cout<<"rule matched"<<endl;
		     $$ = new ParseTreeNode("logic_expression");
			 $$->addChild($1);
			 $$->addChild($2);
			 $$->addChild($3);

			 if($1->dataType == "VOID" || $2->dataType == "VOID"){
				$$->dataType = "VOID";
			 }else{
				$$->dataType = "INT";
			 }
		 
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

			if($1->dataType == "VOID" || $3->dataType == "VOID"){
				$$->dataType = "VOID";
			}else{
				$$->dataType = "INT";
			}
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

			if($1->dataType == "VOID" || $3->dataType == "VOID"){
				$$->dataType = "VOID";
			}else if($1->dataType == "FLOAT" || $3->dataType == "FLOAT"){
				$$->dataType = "FLOAT";
			}else{
				$$->dataType = "INT";
			}
            
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
        
		//cout<<"yoo what is my lexeme "<<$3->lastFoundLexeme<<endl;
		string mulop = $2->lexeme ;

		if($1->dataType == "VOID" || $3->dataType == "VOID"){
			// cannot use arithmatic operator on void
			$$->dataType = "VOID";
		}else if($3->lastFoundLexeme=="0" && (mulop == "%" || mulop == "/") ){
			string error = "Warning: division by zero i=0f=1Const=0" ;
			errorFileWriter(error,$3->startLine);
			if(mulop=="%"){
				$$->dataType = "INT";
			}else{
				$$->dataType = $1->dataType ;
			}
		}else if(mulop=="%" && ($1->dataType == "FLOAT" || $3->dataType == "FLOAT")){
			string error = "Operands of modulus must be integers ";
			errorFileWriter(error,$1->startLine);
			$$->dataType = "INT";
		}else if($1->dataType == "FLOAT" || $3->dataType == "FLOAT"){
			$$->dataType = "FLOAT";
		}else{
			$$->dataType = $1->dataType ;
		}

		
		}
     ;

unary_expression : ADDOP unary_expression {
	logFileWriter("unary_expression","ADDOP unary_expression");
	$$ = new ParseTreeNode("unary_expression");
	$$->addChild($1);
	$$->addChild($2);
	$$->dataType = $2->dataType ;

	} 
		 | NOT unary_expression {
			logFileWriter("unary_expression","NOT unary_expression");
			$$ = new ParseTreeNode("unary_expression");
			$$->addChild($1);
			$$->addChild($2);
            $$->dataType = $2->dataType ;

			// if($2->dataType == "VOID"){
			// 	string error = "Cannot use unary operator on void type";
			// 	errorFileWriter(error,$2->startLine);
			// }
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
		//cout<<"matching the rule"<<endl;
		$$->addChild($1);
		//cout<<"child 1 added"<<endl;
		$$->addChild($2);
		//cout<<"child 2 added"<<endl;
		$$->addChild($3);
		//cout<<"child 3 added"<<endl;
		$$->addChild($4);
		//cout<<"child 4 added"<<endl;

		SymbolInfo* symbol = symbolTable.lookup($1->lexeme);

		if(symbol == nullptr || (!symbol->isFunction) ){
			string error = "Undeclared function '"+$1->lexeme+"'";
			errorFileWriter(error,$1->startLine);
		}else{
			//our symbol is a function 
			FunctionInfo * func = (FunctionInfo*)symbol ;
			//but what are its arguments!!! already stored in argList ^_^

			//cout<<"my size"<<argList.size()<<endl;

			//let's check for wrong number of arguments
			int paramCount = func->getNumberOfParameters();
			int argCount = argList.size();
			bool mismatch = false;
            string error ;
			if(argCount<paramCount){
				mismatch = true;
                error = "Too few arguments to function '"+$1->lexeme+"'";
				errorFileWriter(error,$1->startLine);
			}else if(argCount>paramCount){
                mismatch = true;
                error = "Too many arguments to function '"+$1->lexeme+"'";
				errorFileWriter(error,$1->startLine);
			}else{
				//let's see the paramList now ;
                int i=0 ;

				for(const auto &args : argList){

					if(args != func->findParamAtIndex(i)){
						bool mismatch = true ;
						error = "Type mismatch for argument "+to_string(i+1)+ " of '"+$1->lexeme+"'";
						errorFileWriter(error,$1->startLine);
					}
					i++ ;
				}
			}
			if(!mismatch){
				$$->dataType = func->getReturnType() ;
			}
		}

		argList.clear();
		
		}
	| LPAREN expression RPAREN {
		logFileWriter("factor","LPAREN expression RPAREN");
		$$ = new ParseTreeNode("factor");
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->dataType = $2->dataType;
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
			  |    {
				//argument list can be empty but argument list is a valid node
				$$ = new ParseTreeNode("argument_list");
			       }
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

