flex -o lex.yy.c 2005037.l
g++ lex.yy.c -lfl -o lex.yy.out
./lex.yy.out input.txt