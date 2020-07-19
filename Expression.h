#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <string>

#include "Enums.h"
#include "globals.h"
#include "Value.h"

class Subroutine;
class Symbol;

class Expression
{
public:
	void log_calls(std::string msg);

	static const std::vector<std::string> builtin;

	Subroutine* sub;
	std::vector<Expression*> expressions;
	std::vector<Expression*> block;
	ExprType exprType;
	BinOp op;

	Value* value;
	std::string functionName;

	Expression(Subroutine* sub, ExprType exprType);
	Expression(Subroutine* sub, ExprType exprType, BinOp op);

	Expression* execute();
	void executeDefinition();
	void executeAssignment();
	Expression* executeFunctionCall();
	Expression* executeBinOp();
	void executeIfStatement();
	void executeForLoop();
	void executePrint();

	void setScope(Subroutine* scope);

	Expression* binOpEquals();

	std::string convertDefinition(Symbol* symbol, Expression* expression);
	std::string convertAssignment(Symbol* symbol, Expression* expression);
	std::string convertPrint(Expression* expression);
};

#endif
