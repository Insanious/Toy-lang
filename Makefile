FLAGS = -std=c++11 -g -Wall -Wpedantic

parser: lex.yy.c grammar.tab.o Subroutine.o Symbol.o Expression.o main.cc
	g++ $(FLAGS) -o parser grammar.tab.o Subroutine.o Symbol.o Expression.o lex.yy.c main.cc
grammar.tab.o: grammar.tab.cc
	g++ $(FLAGS) -c grammar.tab.cc

grammar.tab.cc: grammar.yy
	bison grammar.yy -v
lex.yy.c: lexer.ll grammar.tab.cc
	flex lexer.ll
clean:
	rm -f grammar.tab.* lex.yy.c* parser grammar.output *.o

Subroutine.o: Subroutine.cpp Subroutine.h
	g++ $(FLAGS) -c Subroutine.cpp

Expression.o: Expression.cpp Expression.h
	g++ $(FLAGS) -c Expression.cpp

Symbol.o: Symbol.cpp Symbol.h
	g++ $(FLAGS) -c Symbol.cpp

re:
	make clean && make

rerun:
	make clean && make && ./parser test.txt

run:
	make && ./parser test.txt
