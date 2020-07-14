#include "Symbol.h"
#include "Expression.h"

Symbol::Symbol(DataType dataType, std::string name)
{
	this->dataType = dataType;
	this->name = name;
	next = nullptr;
	expression = nullptr;
}
