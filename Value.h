#ifndef VALUE_H
#define VALUE_H

#include <string>
#include "Enums.h"

class Value
{
public:
	DataType dataType;
	std::string name;
	union Immediate
	{
		int i;
		bool b;
	} immediate;
	std::string str;

	Value(DataType dataType) { this->dataType = dataType; }
	Value(const Value &other)
	{
		this->dataType = other.dataType;
		this->name = other.name;
		this->immediate = other.immediate;
		this->str = other.str;
	}
};

#endif
