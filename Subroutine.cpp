#include "Subroutine.h"
#include "Expression.h"

void Subroutine::fatal(std::string msg)
{
	std::cout << "FATAL_SUB:\t" << msg << '\n';
	exit(0);
}

void Subroutine::log(std::string msg)
{
	if (log_output)
		std::cout << "LOG_SUB:\t" << msg << '\n';
}

Subroutine::Subroutine()
{
	parent = nullptr;
}

Subroutine::Subroutine(Subroutine* parent)
{
	this->parent = parent;
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

void Subroutine::addIdentifier(Value* identifier)
{
	if (getIdentifierValue(identifier->name))
		fatal("symbol '" + identifier->name + "' is already defined");

	identifiers.push_back(identifier);
	log("new symbol '" + identifier->name + '\'');
}

Value* Subroutine::getIdentifierValue(std::string name)
{
	Subroutine* scope = this;
	while (scope) // search and return the identifier
	{
		for (auto id : scope->identifiers)
			if (id->name == name)
				return id;

		scope = scope->parent;
	}

	return nullptr;
}

void Subroutine::addFunction(Expression* function)
{
	if (getFunction(function->functionName))
		fatal("function '" + function->functionName + "' is already defined");

	functions.push_back(function);
	log("new function '" + function->functionName + '\'');
}

Expression* Subroutine::getFunction(std::string name)
{
	Subroutine* scope = this;
	while (scope) // search and return the function
	{
		for (auto function : scope->functions)
			if (function->functionName == name)
				return function;

		scope = scope->parent;
	}

	return nullptr;
}
