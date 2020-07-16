#ifndef VALUE_H
#define VALUE_H

#include <string>
#include "Enums.h"

class Value
{
public:
	Value* next;
	DataType dataType;
	std::string name;
	union Immediate
	{
		int i;
		bool b;
	} immediate;
	std::string str;

	Value(DataType dataType) { this->dataType = dataType; this->next = nullptr; }
};

#endif
