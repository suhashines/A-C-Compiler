#ifndef _yy_defines_h_
#define _yy_defines_h_

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
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union YYSTYPE{
	ParseTreeNode* parseTreeNode;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;

#endif /* _yy_defines_h_ */
