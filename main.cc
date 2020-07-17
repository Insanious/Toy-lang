#include <iostream>
#include "grammar.tab.hh"
#include "globals.h"
#include "Subroutine.h"

extern FILE* yyin;
extern Subroutine* global;
bool debug_lex = false;
bool debug_grammar = false;
bool log_output = true;

void yy::parser::error(std::string const&err)
{
	std::cout << "It's one of the bad ones... " << err << " at line " << lineNumber << std::endl;
}

int main(int argc, char **argv)
{
	global = new Subroutine();

	if (argc != 2)
	{
		std::cout << "main: wrong arguments\n";
		return 0;
	}

	yyin = fopen(argv[1], "r");

	yy::parser parser;
	if(!parser.parse())
	{
		global->execute();
	}

	return 0;
}
