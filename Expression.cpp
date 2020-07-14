#include "Expression.h"
#include "Subroutine.h"
#include "Symbol.h"

std::vector<std::string> builtinData = { "print" };
// const vector<int> Foo::something = list_of(3)(5);
const std::vector<std::string> Expression::builtin = builtinData;

Expression::Expression(Subroutine* sub, ExprType exprType)
{
	this->sub = sub;
	this->exprType = exprType;
}

Expression::Expression(Subroutine* sub, ExprType exprType, Symbol* symbol)
{
	this->sub = sub; this->exprType = exprType;
	this->symbol = symbol;
}

Expression* Expression::execute()
{
	switch (exprType)
	{
		case IDENTIFIER:	return sub->lookupSymbol(strValue)->expression;
		case DEFINITION:	executeDefinition();	break;
		case ASSIGNMENT:	executeAssignment();	break;
		case FUNCTIONCALL:	executeFunctionCall();	break;
		case CONSTANT:		return this;
	}

	return nullptr;
}

void Expression::executeDefinition()
{
	// std::cout << "executeDefinition()\n";

	Expression* expr = nullptr;

	if (expressions.size()) // variable is defined and equal to an expression
	{
		expr = expressions[0]->execute();
		if (this->symbol->dataType != expr->dataType)
			Subroutine::fatal("datatype mismatch: expected " + std::to_string(this->symbol->dataType) + " but got " + std::to_string(expr->dataType));

		this->symbol->expression = expr; // assign the expression to the symbol
	}

	sub->addNewSymbol(this->symbol); // add symbol to subroutine
	sub->output += convertDefinition(this->symbol, expr); // append to output
}

void Expression::executeAssignment()
{
	// std::cout << "executeAssignment()\n";

	Symbol* symbol = sub->lookupSymbol(strValue);
	if (!symbol)
		Subroutine::fatal("did not find symbol '" + strValue + "'");

	Expression* expr = expressions[0]->execute();
	if (symbol->dataType != expr->dataType)
		Subroutine::fatal("datatype mismatch: expected " + std::to_string(symbol->dataType) + " but got " + std::to_string(expr->dataType));

	delete symbol->expression; // delete old and ...
	symbol->expression = expr; // .. replace with new

	sub->output += convertAssignment(symbol, expr); // append to output
}

Expression* Expression::executeFunctionCall()
{
	// std::cout << "executeFunctionCall()\n";

	Expression* result = nullptr;
	bool isBuiltin = false;

	for (auto functionName : Expression::builtin)
		if (this->symbol->name == functionName)
		{
			isBuiltin = true;
			break;
		}

	if (isBuiltin)
	{
		if (this->symbol->name == "print")
		{
			executePrint();
			sub->output += convertPrint(expressions[0]->execute()); // reduce expression to single-value expression
		}
	}

	return result;
}

void Expression::executePrint()
{
	// std::cout << "executePrint()\n";

	Expression* expr = expressions[0]->execute();

	switch (expr->dataType)
	{
		case INT:	std::cout << std::to_string(expr->value.i) << '\n';			break;
		case BOOL:	std::cout << (expr->value.b ? "true" : "false") << '\n';	break;
		case FUNCTION:	std::cout << "executePrint() : FUNCTION ";	break;
	}
}

std::string Expression::convertDefinition(Symbol* symbol, Expression* expression)
{
	std::string output;

	switch (symbol->dataType) // output type
	{
		case INT:	output += "int ";	break;
		case BOOL:	output += "bool ";	break;
		case FUNCTION:	output += "function1 ";	break;
	}

	output += symbol->name; // output name

	if (expression) // output assignment value
	{
		output += " = ";

		switch (expression->dataType)
		{
			case INT:	output += std::to_string(expression->value.i);	break;
			case BOOL:	output += expression->value.b ? "true" : "false";	break;
			case FUNCTION:	output += "function2 ";	break;
		}
	}

	output += ";\n";
	return output;
}

std::string Expression::convertAssignment(Symbol* symbol, Expression* expression)
{
	std::string output;

	output += symbol->name; // output name
	output += " = ";

	switch (expression->dataType)
	{
		case INT:	output += std::to_string(expression->value.i);	break;
		case BOOL:	output += expression->value.b ? "true" : "false";	break;
		case FUNCTION:	output += "function3";	break;
	}

	output += ";\n";
	return output;
}

std::string Expression::convertPrint(Expression* expression)
{
	switch (expression->dataType)
	{
		case INT:	return "std::cout << " + std::to_string(expression->value.i) + " << \n;"; break;
		case BOOL:	return "std::cout << " + std::to_string(expression->value.b) + " << \n;"; break;
		case FUNCTION: break;
	}

	return "";
}
