#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <string>

#include "Enums.h"

class Subroutine;
class Symbol;

class Expression
{
public:
	static const std::vector<std::string> builtin;

	Subroutine* sub;
	Symbol* symbol;
	std::vector<Expression*> expressions;
	ExprType exprType;
	DataType dataType;

	union Value
	{
		int i;
		bool b;
	} value;
	std::string strValue;

	Expression(Subroutine* sub, ExprType exprType);
	Expression(Subroutine* sub, ExprType exprType, Symbol* symbol);

	Expression* execute();
	void executeDefinition();
	void executeAssignment();
	Expression* executeFunctionCall();
	void executePrint();

	std::string convertDefinition(Symbol* symbol, Expression* expression);
	std::string convertAssignment(Symbol* symbol, Expression* expression);
	std::string convertPrint(Expression* expression);
};

#endif
