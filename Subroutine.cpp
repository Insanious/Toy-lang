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
}

Subroutine::Subroutine(Subroutine* parent)
{
	this->parent = parent;
}

void Subroutine::clearIdentifiers()
{
	int size = identifiers.size();
	for(int i = 0; i < size; i++)
	{
		std::cout << identifiers[i]->name << '\n';
		delete identifiers[i];
		identifiers[i] = nullptr;
	}
	identifiers.clear();
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
	Subroutine* scope = this;
	while (scope)
	{
		for (auto id : identifiers)
		{
			std::cout << id << '\n';
			if (id->name == identifier->name)
				fatal("symbol '" + identifier->name + "' is already defined");
		}
		scope = scope->parent;
	}

	// Value copy = new Value(*identifier);
	identifiers.push_back(identifier); // make new copy
	// std::cout << "add f, l: " << identifier << ", " << identifiers.back();
	log("new symbol '" + identifier->name + '\'');
}

Value* Subroutine::getIdentifierValue(std::string name)
{
	Subroutine* scope = this;
	while (scope)
	{
		for (auto id : scope->identifiers)
			if (id->name == name)
				return id;

		scope = scope->parent;
	}

	fatal("symbol '" + name + "' is never defined");
	return nullptr;
}

bool Subroutine::identifierExists(std::string name)
{
	Subroutine* scope = this;
	while (scope)
	{
		for (auto id : scope->identifiers)
			if (id->name == name)
				return true;

		scope = scope->parent;
	}

	return false;
}
