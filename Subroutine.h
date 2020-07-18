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
	std::vector<Value*> identifiers;

	std::string output;

	Subroutine();
	Subroutine(Subroutine* parent);

	static void fatal(std::string msg);
	static void log(std::string msg);

	void clearIdentifiers();
	void execute();
	void addIdentifier(Value* identifier);
	Value* getIdentifierValue(std::string name);
	bool identifierExists(std::string name);
};

#endif
