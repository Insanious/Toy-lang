#include "Subroutine.h"
#include "Expression.h"
#include "Symbol.h"

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
	this->parent = nullptr;
	this->symbol = nullptr;
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

void Subroutine::addNewSymbol(Symbol* newSymbol)
{
	Symbol* sym = this->symbol;
	while (sym)
	{
		if (sym->name == newSymbol->name)
			fatal("symbol '" + newSymbol->name + "' is already defined");

		sym = sym->next;
	}

	newSymbol->next = this->symbol;
	this->symbol = newSymbol;
	log("new symbol '" + newSymbol->name + '\'');
}

Symbol* Subroutine::lookupSymbol(std::string name)
{
	Symbol* sym = this->symbol;
	while (sym)
	{
		if (sym->name == name)
			return sym;

		sym = sym->next;
	}

	fatal("symbol '" + name + "' is never defined");
	return nullptr;
}
