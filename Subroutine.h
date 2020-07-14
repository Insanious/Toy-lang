#ifndef SUBROUTINE_H
#define SUBROUTINE_H

#include <vector>
#include <string>
#include <iostream>

#include "Enums.h"
#include "globals.h"

class Expression;
class Symbol;

class Subroutine
{
public:

	std::string name;
	Subroutine* parent;
	std::vector<Expression*> expressions;
	Symbol* symbol;

	std::string output;

	Subroutine();

	static void fatal(std::string msg);
	static void log(std::string msg);

	void execute();
	void addNewSymbol(Symbol* newSymbol);
	Symbol* lookupSymbol(std::string name);
};

#endif
