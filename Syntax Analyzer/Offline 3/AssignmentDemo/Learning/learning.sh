yacc -d -y parser.y
g++ -w -c -o y.o y.tab.c
flex lex.l
g++ -w -c -o l.o lex.yy.c
g++ l.o y.o -lfl -o parser
./parser