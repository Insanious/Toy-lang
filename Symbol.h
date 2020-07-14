#ifndef SYMBOL_H
#define  SYMBOL_H

class Expression;

#include <vector>
#include <string>

#include "Enums.h"

class Symbol
{
public:
	std::string name;
	DataType dataType;
	Symbol* next;
	Expression* expression;

	Symbol(DataType dataType, std::string name);
};

#endif
