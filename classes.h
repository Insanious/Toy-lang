#ifndef CLASSES_H
#define  CLASSES_H

#include <string>
#include <iostream>
#include <vector>

static void fatal(std::string msg)
{
	std::cout << "FATAL: " << msg << '\n';
	exit(0);
}

static void log(std::string msg)
{
	std::cout << "Log: " << msg << '\n';
}

enum ExprType { STATEMENT, IDENTIFIER, DEFINITION, NUMBER };
enum DataType { INT, UNSIGNED };

/* Forward declarations */
class Expression;
class Subroutine;

class Symbol
{
public:
	std::string name;
	DataType dataType;
	Symbol* next;
	Expression* expression;

	Symbol(DataType dataType, std::string name) { this->dataType = dataType; this->name = name; }
};

class Expression
{
public:
	Subroutine* sub;
	Symbol* symbol;
	std::vector<Expression*> expressions;
	ExprType exprType;
	DataType dataType;

	union Value
	{
		int i;
		unsigned u;
	} value;
	std::string strValue;

	Expression(Subroutine* sub, ExprType exprType) { this->sub = sub; this->exprType = exprType; }
	Expression(Subroutine* sub, ExprType exprType, Symbol* symbol) { this->sub = sub; this->exprType = exprType; this->symbol = symbol; }

	Expression* execute(std::string& output)
	{
		switch (exprType)
		{
			case STATEMENT:
			{
				std::cout << "execute stmt\n";
				Expression* res = nullptr;
				for (auto expression : expressions)
					res = expression->execute(output);
				return res;
			}
			case IDENTIFIER:
			{
				std::cout << "execute id\n";
				break;
			}
			case DEFINITION:
			{
				std::cout << "execute def\n";
				if (expressions.size()) // variable is equal to an expression
				{
					Expression* expr = expressions[0]->execute(output);
					if (this->symbol->dataType != expr->dataType)
						fatal("datatype mismatch: expected " + std::to_string(this->symbol->dataType) + " but got " + std::to_string(expr->dataType));

					this->symbol->expression = expr; // assign the expression to the symbol

					sub->addNewSymbol(this->symbol); // add symbol to subroutine
					output += convertDefinition(this->symbol, expr); // append to output
				}
				else // variable is only defined
				{
					sub->addNewSymbol(this->symbol);
					output += convertDefinition(this->symbol, nullptr);
				}

				break;
			}
			case NUMBER:
			{
				std::cout << "execute number\n";
				return this;
			}
		}

		return nullptr;
	}

	std::string convertDefinition(Symbol* symbol, Expression* expression)
	{
		std::string output;

		switch (symbol->dataType) // output type
		{
			case INT:		output += "int ";		break;
			case UNSIGNED:	output += "unsigned ";	break;
		}

		output += symbol->name; // output name

		if (expression) // output assignment value
		{
			output += " = ";

			switch (expression->dataType)
			{
				case INT:		output += std::to_string(expression->value.i);	break;
				case UNSIGNED:	output += std::to_string(expression->value.u);	break;
			}
		}

		output += ";\n";
		return output;
	}
};

class Subroutine
{
public:
	std::string name;
	Subroutine* parent;
	std::vector<Expression*> expressions;
	Symbol* symbol;

	std::string output;

	void execute()
	{
		Expression* res = nullptr;
		for (auto expression : expressions)
			res = expression->execute(output);

		if (res)
			std::cout << "res != 0";

		std::cout << output;
	}

	Symbol* addNewSymbol(Symbol* newSymbol)
	{
		Symbol* sym = this->symbol;
		while (sym)
		{
			if (sym->name == newSymbol.name)
				fatal("symbol '" + newSymbol.name + "' is already defined");

			sym = sym->next;
		}

		newSymbol->next = this->symbol;
		this->symbol = newSymbol;
		log("new symbol '" + name + '\'');

		return sym;
	}
	//
	// Symbol* lookupSymbol(std::string name)
	// {
	// 	Symbol* sym = this->symbol;
	// 	while (sym)
	// 	{
	// 		if (sym->name == name)
	// 			return sym;
	//
	// 		sym = sym->next;
	// 	}
	//
	// 	fatal("symbol '" + name + "' is never defined");
	// 	return nullptr;
	// }
};

#endif
