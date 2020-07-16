#ifndef SUBROUTINE_H
#define SUBROUTINE_H

#include <vector>
#include <string>
#include <iostream>

#include "Value.h"
#include "Enums.h"
#include "globals.h"

class Expression;

class Subroutine
{
public:

	std::string name;
	Subroutine* parent;
	std::vector<Expression*> expressions;
	Value* identifier;

	std::string output;

	Subroutine();

	static void fatal(std::string msg);
	static void log(std::string msg);

	void execute();
	void addNewIdentifier(Value* newIdentifier);
	Value* getIdentifierValue(std::string name);
	bool identifierExists(std::string name);
};

#endif
