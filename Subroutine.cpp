#include "Subroutine.h"
#include "Expression.h"

void Subroutine::fatal(std::string msg)
{
	std::cout << "FATAL:\t\t" << msg << '\n';
	exit(0);
}

void Subroutine::log(std::string msg)
{
	if (log_output)
		std::cout << "LOG:\t\t" << msg << '\n';
}

Subroutine::Subroutine()
{
	parent = nullptr;
	identifier = nullptr;
}

void Subroutine::execute()
{
	Expression* res = nullptr;
	for (auto expression : expressions)
		res = expression->execute();

	if (res)
		std::cout << "res != 0";

	// std::cout << '\n' << output;
}

void Subroutine::addNewIdentifier(Value* newIdentifier)
{
	Value* val = identifier;

	while (val)
	{
		if (val->name == newIdentifier->name)
			fatal("symbol '" + newIdentifier->name + "' is already defined");

		val = val->next;
	}


	newIdentifier->next = identifier;
	identifier = newIdentifier;
	log("new symbol '" + newIdentifier->name + '\'');
}

Value* Subroutine::getIdentifierValue(std::string name)
{
	Value* val = identifier;

	while (val)
	{
		if (val->name == name)
			return val;

		val = val->next;
	}

	fatal("symbol '" + name + "' is never defined");
	return nullptr;
}

bool Subroutine::identifierExists(std::string name)
{
	Value* val = identifier;

	while (val)
	{
		if (val->name == name)
			return true;

		val = val->next;
	}

	return false;
}
