.PHONY = all clean cleanall
FLAGS = -std=c++17 --pedantic -Wextra -Wall

all: compiler

compiler: parser.o data.o code.o lexer.o
	g++ $(FLAGS) -o kompilator parser.o data.o  code.o lexer.o

code.o: code.cpp
	g++ $(FLAGS) -o code.o -c code.cpp
	
data.o: data.cpp
	g++ $(FLAGS) -o data.o -c data.cpp

parser.o: parser.tab.cpp
	g++ $(FLAGS) -o parser.o -c parser.tab.cpp

parser.tab.cpp: parser.ypp
	bison -d parser.ypp
	
lexer.o: lexer.c
	g++ $(FLAGS) -o lexer.o -c lexer.c

lexer.c: lexer.l
	flex -o lexer.c lexer.l	
