#!/bin/bash

# Generate parser C file and header file
yacc -d -y 2005037.y
echo 'Generated the parser C file as well as the header file'

# Compile parser object file
g++ -w -c -o y.o y.tab.c
echo 'Generated the parser object file'

# Generate scanner C file
flex 2005037.l
echo 'Generated the scanner C file'

# Compile scanner object file
g++ -w -c -o l.o lex.yy.c
echo 'Generated the scanner object file'

# Link all object files to generate the executable
g++ y.o l.o -lfl -o 2005037
echo 'All ready, running'

#put input file name here
./2005037 input.c

#generating optimized code

echo 'Generating optimal assembly'
g++ -o optimizer.o 2005037_optimizer.cpp
./optimizer.o
