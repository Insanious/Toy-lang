#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <string>

#include "Value.h"
#include "Enums.h"
#include "globals.h"

class Subroutine;
class Symbol;

class Expression
{
public:
	void log_executions(std::string msg);
	void log_binOps(std::string msg);

	static const std::vector<std::string> builtin;

	Subroutine* scope;
	std::vector<Expression*> expressions;
	std::vector<Expression*> block;
	std::vector<Expression*> arguments;
	ExprType exprType;
	BinOp binOp;

	Value* value;
	std::string functionName;

	Expression(Subroutine* scope, ExprType exprType);
	Expression(Subroutine* scope, ExprType exprType, BinOp binOp);

	static void fatal(std::string msg);
	static void log(std::string msg);

	void checkFunctionArguments();

	Expression* execute();
	Expression*	executeIdentifier();
	void executeDefinition();
	void executeAssignment();
	Expression* executeFunctionCall();
	Expression* executeFunctionDefinition();
	Expression* executeBinOp();
	void executeIfStatement();
	void executeForLoop();
	void executePrint();

	void setScope(Subroutine* newScope);

	Expression* binOpEquals();

	std::string convertDefinition(Symbol* symbol, Expression* expression);
	std::string convertAssignment(Symbol* symbol, Expression* expression);
	std::string convertPrint(Expression* expression);
};

#endif
