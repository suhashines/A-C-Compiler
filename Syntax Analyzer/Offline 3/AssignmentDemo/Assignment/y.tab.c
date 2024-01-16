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

#line 2 "parser.y"
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

/*necessary global variables */
string varType ;

void yyerror(char *s)
{
	/*write your code*/
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

/*here comes the beast who prints the entire parseTree*/

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

    parseFile<<root->name<<" :"<<root->nameList<<"\t<Line : "<<root->startLine <<"-"<<root->endLine<< ">\n";

    for(ParseTreeNode* node:root->children){
        printParseTree(node,space+1);
    }


}


/* necessary functions for recognizing semantic errors */


void handleIdDeclaration(ParseTreeNode* node){
	/*extracting information */

	string lexeme = node->lexeme ;
	string token = node->token;
	string idType = varType ;
	int lineCount = node->startLine;

	/*looking for error */

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








/*////  grammar section ///////////////////////*/


#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 126 "parser.y"
typedef union YYSTYPE{
	ParseTreeNode* parseTreeNode;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 158 "y.tab.c"

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
#define INCOP 283
#define DECOP 284
#define ASSIGNOP 285
#define CONST_INT 286
#define CONST_FLOAT 287
#define LOGICOP 288
#define RELOP 289
#define ADDOP 290
#define BITOP 291
#define MULOP 292
#define ID 293
#define LOWER_THAN_ELSE 294
#define YYERRCODE 256
typedef int YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    1,    1,    2,    2,    2,    4,    4,    5,    5,
    7,    7,    7,    7,    8,    8,    3,    6,    6,    6,
   10,   10,   10,   10,    9,    9,   11,   11,   11,   11,
   11,   11,   11,   11,   11,   12,   12,   14,   14,   13,
   13,   15,   15,   16,   16,   17,   17,   18,   18,   19,
   19,   19,   20,   20,   20,   20,   20,   20,   20,   21,
   21,   22,   22,
};
static const YYINT yylen[] = {                            2,
    1,    2,    1,    1,    1,    1,    6,    5,    6,    5,
    4,    3,    2,    1,    3,    2,    3,    1,    1,    1,
    3,    6,    1,    4,    1,    2,    1,    1,    1,    7,
    5,    7,    5,    5,    3,    1,    2,    1,    4,    1,
    3,    1,    3,    1,    3,    1,    3,    1,    3,    2,
    2,    1,    1,    4,    3,    1,    1,    2,    2,    1,
    0,    3,    1,
};
static const YYINT yydefred[] = {                         0,
   18,   19,   20,    0,    0,    3,    4,    5,    6,    0,
    2,    0,    0,    0,    0,    0,   17,    0,    0,    0,
    0,    0,    0,    8,   10,   13,    0,    0,   24,    0,
    0,    0,    0,    0,    0,    0,    0,   16,   36,   56,
   57,    0,    0,   27,    0,   29,    0,   25,   28,    0,
    0,   40,    0,    0,    0,   48,   52,    7,    9,    0,
    0,    0,    0,    0,    0,    0,    0,   51,    0,   50,
    0,    0,    0,   15,   26,   37,   58,   59,    0,    0,
    0,    0,    0,   11,   22,    0,    0,    0,   35,    0,
   55,   63,    0,    0,    0,   41,   43,    0,    0,   49,
    0,    0,    0,    0,   54,    0,   39,    0,    0,   33,
   34,   62,    0,    0,   32,   30,
};
#if defined(YYDESTRUCT_CALL) || defined(YYSTYPE_TOSTRING)
static const YYINT yystos[] = {                           0,
  263,  265,  267,  296,  297,  298,  299,  300,  301,  302,
  298,  293,  306,  275,  279,  281,  282,  276,  302,  303,
  286,  293,  277,  282,  304,  293,  276,  281,  280,  279,
  257,  259,  260,  268,  273,  274,  275,  278,  282,  286,
  287,  290,  293,  299,  302,  304,  305,  307,  308,  309,
  310,  311,  312,  313,  314,  315,  316,  282,  304,  302,
  286,  275,  275,  275,  309,  275,  310,  315,  309,  315,
  275,  279,  293,  278,  307,  282,  283,  284,  285,  288,
  289,  290,  292,  293,  280,  309,  308,  309,  282,  293,
  276,  311,  317,  318,  309,  311,  312,  313,  314,  315,
  276,  308,  276,  276,  276,  281,  280,  307,  309,  307,
  282,  311,  258,  276,  307,  307,
};
#endif /* YYDESTRUCT_CALL || YYSTYPE_TOSTRING */
static const YYINT yydgoto[] = {                          4,
    5,    6,   44,    8,    9,   45,   20,   46,   47,   13,
   48,   49,   50,   51,   52,   53,   54,   55,   56,   57,
   93,   94,
};
static const YYINT yysindex[] = {                      -136,
    0,    0,    0,    0, -136,    0,    0,    0,    0, -286,
    0, -239, -149, -166, -271, -263,    0, -250, -260, -226,
 -232, -228, -184,    0,    0,    0, -215, -136,    0, -223,
 -198, -183, -179, -233, -171, -233, -233,    0,    0,    0,
    0, -233, -201,    0, -169,    0, -152,    0,    0, -168,
 -165,    0, -160, -140, -156,    0,    0,    0,    0, -151,
 -134, -233,  -79, -233, -138, -137, -132,    0, -115,    0,
 -233, -233, -114,    0,    0,    0,    0,    0, -233, -233,
 -233, -233, -233,    0,    0, -108,  -79, -102,    0, -100,
    0,    0,  -95,  -93,  -87,    0,    0,  -90, -156,    0,
  -88, -233,  -88,  -85,    0, -233,    0,  -57,  -72,    0,
    0,    0,  -88,  -88,    0,    0,
};
static const YYINT yyrindex[] = {                         0,
    0,    0,    0,    0,  206,    0,    0,    0,    0,    0,
    0, -122,    0,    0,    0,    0,    0,    0, -196,    0,
    0, -118,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0, -264,    0,    0,    0,    0,    0,    0,    0,
 -224,    0, -194,  -46,  -98,    0,    0,    0,    0, -181,
    0,    0,    0,    0,    0,    0,  -64,    0,    0,    0,
  -67,    0, -122,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -66,    0,    0,    0,  -32,  -49,    0,
    0,    0,    0,    0,    0,    0,    0, -120,    0,    0,
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
    0,    0,    0,    0,    0,    0,
};
#endif
static const YYINT yygindex[] = {                         0,
    0,  208,  112,    0,    0,    9,    0,   -5,    0,    0,
  -42,  -53,  -33,  -36,  -68,  135,  138,  139,  -34,    0,
    0,    0,
};
#define YYTABLESIZE 256
static const YYINT yytable[] = {                         67,
   65,   68,   92,   69,   75,   67,   12,   70,   10,   87,
   96,   38,   25,   10,   21,   38,   38,   38,   38,   38,
   38,   59,   19,   38,   38,   38,   23,   38,   86,   22,
   88,   24,   26,  102,   67,   14,   60,  112,   95,   15,
   36,   37,   67,   67,   67,   67,   67,   29,  100,   27,
   30,   53,   40,   41,   28,   53,   42,   53,  108,   43,
  110,   23,   61,   53,   53,   53,   58,   53,  109,   67,
  115,  116,   31,   71,   32,   33,   62,   72,    1,   14,
    2,   42,    3,   34,   14,   42,   42,   42,   35,   36,
   37,   63,   23,   38,   12,   64,    1,   39,    2,   12,
    3,   40,   41,   66,   31,   42,   32,   33,   43,   18,
    1,    7,    2,   76,    3,   34,    7,   77,   78,   79,
   35,   36,   37,   73,   23,   74,    1,   80,    2,   39,
    3,   16,   17,   40,   41,   83,   31,   42,   31,   31,
   43,   84,   31,   89,   31,   85,   31,   31,   81,   82,
   77,   78,   31,   31,   31,   90,   31,   31,   23,   23,
   91,   31,   21,   21,   15,   31,   31,  101,   31,   31,
   32,   33,   31,  103,    1,  104,    2,   46,    3,   34,
  105,   46,   46,   46,   35,   36,   37,  106,   23,   46,
   46,   46,  107,   39,   36,   37,  111,   40,   41,   82,
  113,   42,   39,  114,   43,    1,   40,   41,   61,   60,
   42,   53,   11,   43,   97,   53,   53,   53,   98,    0,
   99,    0,    0,   53,   53,   53,   47,   53,    0,   44,
   47,   47,   47,   44,   44,   44,    0,    0,   47,   47,
   47,   44,    0,   45,    0,    0,    0,   45,   45,   45,
    0,    0,    0,    0,    0,   45,
};
static const YYINT yycheck[] = {                         36,
   34,   36,   71,   37,   47,   42,  293,   42,    0,   63,
   79,  276,   18,    5,  286,  280,  281,  282,  283,  284,
  285,   27,   14,  288,  289,  290,  277,  292,   62,  293,
   64,  282,  293,   87,   71,  275,   28,  106,   72,  279,
  274,  275,   79,   80,   81,   82,   83,  280,   83,  276,
  279,  276,  286,  287,  281,  280,  290,  282,  101,  293,
  103,  277,  286,  288,  289,  290,  282,  292,  102,  106,
  113,  114,  257,  275,  259,  260,  275,  279,  263,  276,
  265,  276,  267,  268,  281,  280,  281,  282,  273,  274,
  275,  275,  277,  278,  276,  275,  263,  282,  265,  281,
  267,  286,  287,  275,  257,  290,  259,  260,  293,  276,
  263,    0,  265,  282,  267,  268,    5,  283,  284,  285,
  273,  274,  275,  293,  277,  278,  263,  288,  265,  282,
  267,  281,  282,  286,  287,  292,  257,  290,  259,  260,
  293,  293,  263,  282,  265,  280,  267,  268,  289,  290,
  283,  284,  273,  274,  275,  293,  277,  278,  281,  282,
  276,  282,  281,  282,  279,  286,  287,  276,  257,  290,
  259,  260,  293,  276,  263,  276,  265,  276,  267,  268,
  276,  280,  281,  282,  273,  274,  275,  281,  277,  288,
  289,  290,  280,  282,  274,  275,  282,  286,  287,  290,
  258,  290,  282,  276,  293,    0,  286,  287,  276,  276,
  290,  276,    5,  293,   80,  280,  281,  282,   81,   -1,
   82,   -1,   -1,  288,  289,  290,  276,  292,   -1,  276,
  280,  281,  282,  280,  281,  282,   -1,   -1,  288,  289,
  290,  288,   -1,  276,   -1,   -1,   -1,  280,  281,  282,
   -1,   -1,   -1,   -1,   -1,  288,
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
   -1,   -1,   -1,   -1,   -1,
};
#endif
#define YYFINAL 4
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 294
#define YYUNDFTOKEN 319
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
"COMMA","SEMICOLON","INCOP","DECOP","ASSIGNOP","CONST_INT","CONST_FLOAT",
"LOGICOP","RELOP","ADDOP","BITOP","MULOP","ID","LOWER_THAN_ELSE","$accept",
"start","program","unit","var_declaration","func_declaration","func_definition",
"type_specifier","parameter_list","compound_statement","statements",
"declaration_list","statement","expression_statement","expression","variable",
"logic_expression","rel_expression","simple_expression","term",
"unary_expression","factor","argument_list","arguments","illegal-symbol",
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
"expression_statement : SEMICOLON",
"expression_statement : expression SEMICOLON",
"variable : ID",
"variable : ID LTHIRD expression RTHIRD",
"expression : logic_expression",
"expression : variable ASSIGNOP logic_expression",
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
#line 380 "parser.y"
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

#line 676 "y.tab.c"

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
#line 153 "parser.y"
	{
		logFileWriter("start","program");
		summaryWriter();
		yyval.parseTreeNode = new ParseTreeNode("start");
		yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		printParseTree(yyval.parseTreeNode,0);
	}
#line 1355 "y.tab.c"
break;
case 2:
#line 162 "parser.y"
	{
	logFileWriter("program","program unit");
}
#line 1362 "y.tab.c"
break;
case 3:
#line 165 "parser.y"
	{logFileWriter("program","unit");
	
	yyval.parseTreeNode = new ParseTreeNode("program");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	
	}
#line 1372 "y.tab.c"
break;
case 4:
#line 173 "parser.y"
	{
	logFileWriter("unit","var_declaration");
	yyval.parseTreeNode = new ParseTreeNode("unit");
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
}
#line 1381 "y.tab.c"
break;
case 5:
#line 178 "parser.y"
	{
		logFileWriter("unit","func_declaration");
	 }
#line 1388 "y.tab.c"
break;
case 6:
#line 181 "parser.y"
	{
		logFileWriter("unit","func_definition");
	 }
#line 1395 "y.tab.c"
break;
case 7:
#line 186 "parser.y"
	{
	logFileWriter("func_declaration","type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
}
#line 1402 "y.tab.c"
break;
case 8:
#line 189 "parser.y"
	{
			logFileWriter("func_declaration","type_specifier ID LPAREN RPAREN SEMICOLON");
		}
#line 1409 "y.tab.c"
break;
case 9:
#line 194 "parser.y"
	{
	logFileWriter("func_definition","type_specifier ID LPAREN parameter_list RPAREN compound_statement");
}
#line 1416 "y.tab.c"
break;
case 10:
#line 197 "parser.y"
	{
			logFileWriter("func_definition","type_specifier ID LPAREN RPAREN compound_statement");
		}
#line 1423 "y.tab.c"
break;
case 11:
#line 203 "parser.y"
	{logFileWriter("parameter_list","parameter_list COMMA type_specifier ID");}
#line 1428 "y.tab.c"
break;
case 12:
#line 204 "parser.y"
	{logFileWriter("parameter_list","parameter_list COMMA type_specifier");}
#line 1433 "y.tab.c"
break;
case 13:
#line 205 "parser.y"
	{logFileWriter("parameter_list","type_specifier ID");}
#line 1438 "y.tab.c"
break;
case 14:
#line 206 "parser.y"
	{logFileWriter("parameter_list","type_specifier");}
#line 1443 "y.tab.c"
break;
case 15:
#line 210 "parser.y"
	{logFileWriter("compound_statement","LCURL statements RCURL");}
#line 1448 "y.tab.c"
break;
case 16:
#line 211 "parser.y"
	{logFileWriter("compound_statement","LCURL RCURL");}
#line 1453 "y.tab.c"
break;
case 17:
#line 214 "parser.y"
	{logFileWriter("var_declaration","type_specifier declaration_list SEMICOLON");

yyval.parseTreeNode = new ParseTreeNode("var_declaration");
yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);

}
#line 1465 "y.tab.c"
break;
case 18:
#line 224 "parser.y"
	{
							logFileWriter("type_specifier","INT");
							varType = "int" ;
							yyval.parseTreeNode = new ParseTreeNode("type_specifier");
							yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);


					   }
#line 1477 "y.tab.c"
break;
case 19:
#line 232 "parser.y"
	{
							logFileWriter("type_specifier", "FLOAT");
							varType = "float" ;
							yyval.parseTreeNode = new ParseTreeNode("type_specifier");
							yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
					   }
#line 1487 "y.tab.c"
break;
case 20:
#line 238 "parser.y"
	{
							logFileWriter("type_specifier", "VOID");
							varType = "void" ;
							yyval.parseTreeNode = new ParseTreeNode("type_specifier");
							yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
		}
#line 1497 "y.tab.c"
break;
case 21:
#line 246 "parser.y"
	{
	logFileWriter("declaration_list","declaration_list COMMA ID");
	
	handleIdDeclaration(yystack.l_mark[0].parseTreeNode);

	yyval.parseTreeNode = new ParseTreeNode("declaration_list");

	yyval.parseTreeNode->addChild(yystack.l_mark[-2].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[-1].parseTreeNode);
	yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
	

}
#line 1514 "y.tab.c"
break;
case 22:
#line 259 "parser.y"
	{
			logFileWriter("declaration_list","declaration_list COMMA ID LSQUARE CONST_INT RSQUARE");
		  }
#line 1521 "y.tab.c"
break;
case 23:
#line 262 "parser.y"
	{
			logFileWriter("declaration_list","ID");
			handleIdDeclaration(yystack.l_mark[0].parseTreeNode);

			yyval.parseTreeNode = new ParseTreeNode("declaration_list");
			yyval.parseTreeNode->addChild(yystack.l_mark[0].parseTreeNode);
			
			}
#line 1533 "y.tab.c"
break;
case 24:
#line 270 "parser.y"
	{
			logFileWriter("declaration_list","ID LTHIRD CONST_INT RTHIRD");
		  }
#line 1540 "y.tab.c"
break;
case 25:
#line 275 "parser.y"
	{logFileWriter("statements","statement");}
#line 1545 "y.tab.c"
break;
case 26:
#line 276 "parser.y"
	{
		logFileWriter("statements","statements statement");
	   }
#line 1552 "y.tab.c"
break;
case 27:
#line 281 "parser.y"
	{
	logFileWriter("statement","var_declaration");
}
#line 1559 "y.tab.c"
break;
case 28:
#line 284 "parser.y"
	{
		logFileWriter("statement","expression_statement");
	  }
#line 1566 "y.tab.c"
break;
case 29:
#line 287 "parser.y"
	{
		logFileWriter("statement","compound_statement");
	  }
#line 1573 "y.tab.c"
break;
case 30:
#line 290 "parser.y"
	{
		logFileWriter("statement","FOR LPAREN expression_statement expression_statement expression RPAREN statement");
	  }
#line 1580 "y.tab.c"
break;
case 31:
#line 293 "parser.y"
	{
		logFileWriter("statement","IF LPAREN expression RPAREN statement");
	  }
#line 1587 "y.tab.c"
break;
case 32:
#line 296 "parser.y"
	{
		logFileWriter("statement","IF LAPAREN expression RPAREN statement ELSE statement");
	  }
#line 1594 "y.tab.c"
break;
case 33:
#line 299 "parser.y"
	{
		logFileWriter("statement","WHILE LPAREN expression RPAREN statement");
	  }
#line 1601 "y.tab.c"
break;
case 34:
#line 302 "parser.y"
	{
		logFileWriter("statement","PRINTLN LPAREN ID RPAREN SEMICOLON");
	  }
#line 1608 "y.tab.c"
break;
case 35:
#line 305 "parser.y"
	{
		logFileWriter("statement","RETURN expression SEMICOLON");
	  }
#line 1615 "y.tab.c"
break;
case 36:
#line 310 "parser.y"
	{logFileWriter("expression_statement","SEMICOLON");}
#line 1620 "y.tab.c"
break;
case 37:
#line 311 "parser.y"
	{logFileWriter("expression_statement","expression SEMICOLON");}
#line 1625 "y.tab.c"
break;
case 38:
#line 314 "parser.y"
	{logFileWriter("variable","ID");}
#line 1630 "y.tab.c"
break;
case 39:
#line 315 "parser.y"
	{
		logFileWriter("variable","ID LTHIRD expression RTHIRD");
	 }
#line 1637 "y.tab.c"
break;
case 40:
#line 320 "parser.y"
	{logFileWriter("expression","logic_expression");}
#line 1642 "y.tab.c"
break;
case 41:
#line 321 "parser.y"
	{
		logFileWriter("expression","variable ASSIGNOP logic_expression");
	   }
#line 1649 "y.tab.c"
break;
case 42:
#line 326 "parser.y"
	{
	logFileWriter("logic_expression","rel_expression");
}
#line 1656 "y.tab.c"
break;
case 43:
#line 329 "parser.y"
	{
			logFileWriter("logic_expression","rel_expression LOGICOP rel_expression");
		 }
#line 1663 "y.tab.c"
break;
case 44:
#line 334 "parser.y"
	{
	logFileWriter("rel_expression","simple_expression");
}
#line 1670 "y.tab.c"
break;
case 45:
#line 337 "parser.y"
	{
			logFileWriter("rel_expression","simple_expression RELOP simple_expression");
		}
#line 1677 "y.tab.c"
break;
case 46:
#line 342 "parser.y"
	{logFileWriter("simple_expression","term");}
#line 1682 "y.tab.c"
break;
case 47:
#line 343 "parser.y"
	{
			logFileWriter("simple_expression","simple_expression ADDOP term");
		  }
#line 1689 "y.tab.c"
break;
case 48:
#line 348 "parser.y"
	{logFileWriter("term","unary_expression");}
#line 1694 "y.tab.c"
break;
case 49:
#line 349 "parser.y"
	{logFileWriter("term","term MULOP unary_expression");}
#line 1699 "y.tab.c"
break;
case 50:
#line 352 "parser.y"
	{logFileWriter("unary_expression","ADDOP unary_expression");}
#line 1704 "y.tab.c"
break;
case 51:
#line 353 "parser.y"
	{logFileWriter("unary_expression","NOT unary_expression");}
#line 1709 "y.tab.c"
break;
case 52:
#line 354 "parser.y"
	{logFileWriter("unary_expression","factor");}
#line 1714 "y.tab.c"
break;
case 53:
#line 357 "parser.y"
	{logFileWriter("factor","variable");}
#line 1719 "y.tab.c"
break;
case 54:
#line 358 "parser.y"
	{logFileWriter("factor","ID LPAREN argument_list RPAREN");}
#line 1724 "y.tab.c"
break;
case 55:
#line 359 "parser.y"
	{logFileWriter("factor","LPAREN expression RPAREN");}
#line 1729 "y.tab.c"
break;
case 56:
#line 360 "parser.y"
	{logFileWriter("factor","CONST_INT");}
#line 1734 "y.tab.c"
break;
case 57:
#line 361 "parser.y"
	{logFileWriter("factor","CONST_FLOAT");}
#line 1739 "y.tab.c"
break;
case 58:
#line 362 "parser.y"
	{logFileWriter("factor","variable INCOP");}
#line 1744 "y.tab.c"
break;
case 59:
#line 363 "parser.y"
	{logFileWriter("factor","variable DECOP");}
#line 1749 "y.tab.c"
break;
case 60:
#line 366 "parser.y"
	{logFileWriter("argument_list","arguments");}
#line 1754 "y.tab.c"
break;
case 62:
#line 370 "parser.y"
	{
	logFileWriter("arguments","arguments COMMA logic_expression");
}
#line 1761 "y.tab.c"
break;
case 63:
#line 373 "parser.y"
	{
			logFileWriter("arguments","logic_expression");
		  }
#line 1768 "y.tab.c"
break;
#line 1770 "y.tab.c"
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
