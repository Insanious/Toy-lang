#include "Expression.h"
#include "Subroutine.h"

std::vector<std::string> builtinData = { "print" };
const std::vector<std::string> Expression::builtin = builtinData;

void Expression::fatal(std::string msg)
{
	std::cout << "FATAL_EXPR:\t" << msg << '\n';
	exit(0);
}

void Expression::log(std::string msg)
{
	if (log_output)
		std::cout << "LOG_EXPR:\t" << msg << '\n';
}

void Expression::log_executions(std::string msg)
{
	if (debug_executions)
		std::cout << "CALL: " << msg << '\n';
}

void Expression::log_binOps(std::string msg)
{
	if (debug_binOps)
		std::cout << "BINOP: " << msg << '\n';
}

Expression::Expression(Subroutine* scope, ExprType exprType)
{
	this->scope = scope;
	this->exprType = exprType;
}

Expression::Expression(Subroutine* scope, ExprType exprType, BinOp binOp)
{
	this->scope = scope;
	this->exprType = exprType;
	this->binOp = binOp;
}

Expression* Expression::execute()
{
	log_executions("execute() " + std::to_string(int(exprType)));

	switch (exprType)
	{
		case ExprType::FUNCTIONDEFINITION:	executeFunctionDefinition();	break;
		case ExprType::FUNCTIONCALL:		executeFunctionCall();			break;
		case ExprType::IDENTIFIER:			return executeIdentifier();
		case ExprType::DEFINITION:			executeDefinition();			break;
		case ExprType::ASSIGNMENT:			executeAssignment();			break;
		case ExprType::CONSTANT:			return this;
		case ExprType::BINOP:				return executeBinOp();
		case ExprType::IFSTMT:				executeIfStatement();			break;
		case ExprType::FORLOOP:				executeForLoop();				break;
	}

	return nullptr;
}

Expression* Expression::executeIdentifier()
{
	Value* identifierValue = scope->getIdentifierValue(value->name);
	if (!identifierValue)
		fatal("symbol '" + value->name + "' is never defined");

	value = identifierValue;
	return this;
}

void Expression::executeDefinition()
{
	log_executions("executeDefinition()");

	Expression* expr = nullptr;

	if (expressions.size()) // variable is defined and equal to an expression
	{
		expr = expressions[0]->execute();
		if (value->dataType != expr->value->dataType)
			fatal("datatype mismatch: expected " + std::to_string((int)value->dataType) + " but got " + std::to_string((int)expr->value->dataType));

		switch (value->dataType) // assign the expression to the identifier
		{
			case DataType::INT:			value->immediate.i = expr->value->immediate.i;	break;
			case DataType::BOOL:		value->immediate.b = expr->value->immediate.b;	break;
			case DataType::STRING:		value->str = expr->value->str;					break;
			case DataType::IDENTIFIER:	std::cout << "executePrint() : IDENTIFIER";	break;
			case DataType::FUNCTION:	std::cout << "executePrint() : FUNCTION";	break;
		}
	}

	scope->addIdentifier(value); // add symbol to scope
	// scope->output += convertDefinition(this->symbol, expr); // append to output
}

void Expression::executeAssignment()
{
	log_executions("executeAssignment()");

	Value* currentValue = scope->getIdentifierValue(value->name);
	if (!currentValue)
		fatal("identifier '" + value->name + "' is never defined");

	Expression* expr = expressions[0]->execute();
	if (currentValue->dataType != expr->value->dataType)
		fatal("datatype mismatch: expected " + std::to_string((int)currentValue->dataType) + " but got " + std::to_string((int)expr->value->dataType));

	switch (currentValue->dataType) // assign the expression to the identifier
	{
		case DataType::INT:			currentValue->immediate.i = expr->value->immediate.i;	break;
		case DataType::BOOL:		currentValue->immediate.b = expr->value->immediate.b;	break;
		case DataType::STRING:		currentValue->str = expr->value->str;					break;
		case DataType::IDENTIFIER:	std::cout << "executeAssignment() : IDENTIFIER";		break;
		case DataType::FUNCTION:	std::cout << "executeAssignment() : FUNCTION";			break;
	}

	// scope->output += convertAssignment(symbol, expr); // append to output
}

Expression* Expression::executeFunctionDefinition()
{
	log_executions("executeFunctionDefinition()");

	scope->addFunction(this);
	return nullptr;
}

Expression* Expression::executeFunctionCall()
{
	log_executions("executeFunctionCall()");

	Expression* result = nullptr;
	bool isBuiltin = false;

	for (auto name : Expression::builtin)
		if (functionName == name)
		{
			isBuiltin = true;
			break;
		}

	if (isBuiltin)
	{
		if (functionName == "print")
		{
			executePrint();
			// scope->output += convertPrint(expressions[0]->execute()); // reduce expression to single-value expression
		}
	}
	else
	{
		Expression* function = scope->getFunction(functionName);
		if (!function)
			fatal("function '" + functionName + "' is never defined");

		if (function->arguments.size() != this->arguments.size())
		{
			if (function->arguments.size() == 1)
				fatal("expected " + std::to_string((int)function->arguments.size()) + " argument but got " + std::to_string((int)this->arguments.size()) +  " in function '" + functionName + "'");
			fatal("expected " + std::to_string((int)function->arguments.size()) + " arguments but got " + std::to_string((int)this->arguments.size()) +  " in function '" + functionName + "'");
		}

		int nrOfArguments = function->arguments.size();
		for (int i = 0; i < nrOfArguments; i++)
			if (function->arguments[i]->value->dataType != this->arguments[i]->execute()->value->dataType)
				fatal("expected argument " + std::to_string(i) +  " to be of type " + std::to_string((int)function->arguments[i]->value->dataType) + " but got " + std::to_string((int)this->arguments[i]->execute()->value->dataType) +  " in function '" + functionName + "'");

		Subroutine newScope = Subroutine(scope);
		function->setScope(&newScope);

		for (int i = 0; i < nrOfArguments; i++)
		{
			// execute definition with value of this->argument->value
			Value* argValue = this->arguments[i]->execute()->value;

			function->arguments[i]->value->immediate = argValue->immediate;
			function->arguments[i]->value->str = argValue->str;
			function->arguments[i]->execute();
		}

		for (auto stmt : function->block)
			stmt->execute();
	}

	return result;
}

Expression* Expression::executeBinOp()
{
	log_executions("executeBinOp()");
	switch (binOp)
	{
		case BinOp::EQUALS: return binOpEquals();
	}

	return nullptr;
}

void Expression::executeIfStatement()
{
	log_executions("executeIfStatement()");

	Expression* expr = expressions[0]->execute();

	if (expr->exprType != ExprType::CONSTANT)
		fatal("expected ExprType::CONSTANT but got " + std::to_string((int)expr->exprType));
	if (expr->value->dataType != DataType::BOOL)
		fatal("expected DataType::BOOL but got " + std::to_string((int)expr->value->dataType));

	if (expr->value->immediate.b)
	{
		Subroutine newScope = Subroutine(scope);

		this->setScope(&newScope); // recursively set the scope of all subexpressions
		int size = block.size();
		for (int i = 0; i < size; i++)
			block[i]->execute(); // execute all statements
	}
}

void Expression::executeForLoop()
{
	log_executions("executeForLoop()");

	if (expressions.size() == 1) // bracketloop
	{
		Expression* condition = expressions[0];
		if (condition->value->dataType != DataType::INT)
			fatal("expected DataType::INT but got " + std::to_string((int)condition->value->dataType));

		int size = block.size();
		for (int i = 0; i < condition->value->immediate.i; i++)
		{
			Subroutine newScope = Subroutine(scope);
			this->setScope(&newScope);
			for (int j = 0; j < size; j++)
				block[j]->execute();
		}
	}
}

void Expression::executePrint()
{
	log_executions("executePrint()");

	Expression* expr = arguments[0]->execute();

	switch (expr->value->dataType)
	{
		case DataType::INT:			std::cout << std::to_string(expr->value->immediate.i) << '\n';		break;
		case DataType::BOOL:		std::cout << (expr->value->immediate.b ? "true" : "false") << '\n';	break;
		case DataType::STRING:		std::cout << expr->value->str << '\n';								break;
		case DataType::IDENTIFIER:	std::cout << "executePrint() : IDENTIFIER";							break;
		case DataType::FUNCTION:	std::cout << "executePrint() : FUNCTION";							break;
	}
}

void Expression::setScope(Subroutine* newScope)
{
	for (auto statement : block)
	{
		statement->scope = newScope;
		statement->setScope(newScope);
	}

	for (auto arg : arguments)
	{
		arg->scope = newScope;
		arg->setScope(newScope);
	}
}

Expression* Expression::binOpEquals()
{
	log_binOps("binOpEquals()");

	Value* left = expressions[0]->execute()->value;
	Value* right = expressions[1]->execute()->value;

	if (left->dataType != right->dataType)
		fatal("expected " + std::to_string((int)left->dataType) + " but got " + std::to_string((int)right->dataType));

	Expression* expr = new Expression(scope, ExprType::CONSTANT);
	expr->value = new Value(DataType::BOOL);

	switch (left->dataType)
	{
		case DataType::INT:			expr->value->immediate.b = (left->immediate.i == right->immediate.i);	break;
		case DataType::BOOL:		expr->value->immediate.b = (left->immediate.b == right->immediate.b);	break;
		case DataType::STRING:		expr->value->immediate.b = (left->str == right->str);					break;
		case DataType::IDENTIFIER:	fatal("binOpEquals: left->dataType == IDENTIFIER");			break;
		case DataType::FUNCTION:	fatal("binOpEquals: left->dataType == FUNCTION");			break;
	}

	return expr;
}

// std::string Expression::convertDefinition(Symbol* symbol, Expression* expression)
// {
// 	std::string output;
//
// 	switch (symbol->dataType) // output type
// 	{
// 		case DataType::INT:			output += "int ";	break;
// 		case DataType::BOOL:		output += "bool ";	break;
// 		case DataType::IDENTIFIER:	output += "identifier1 ";	break;
// 		case DataType::FUNCTION:	output += "function1 ";	break;
// 	}
//
// 	output += symbol->name; // output name
//
// 	if (expression) // output assignment value
// 	{
// 		output += " = ";
//
// 		switch (expression->value->dataType)
// 		{
// 			case DataType::INT:			output += std::to_string(expression->value->immediate.i);	break;
// 			case DataType::BOOL:		output += expression->value->immediate.b ? "true" : "false";	break;
// 			case DataType::IDENTIFIER:	output += "identifier2 ";	break;
// 			case DataType::FUNCTION:	output += "function2 ";	break;
// 		}
// 	}
//
// 	output += ";\n";
// 	return output;
// }

// std::string Expression::convertAssignment(Symbol* symbol, Expression* expression)
// {
// 	std::string output;
//
// 	output += symbol->name; // output name
// 	output += " = ";
//
// 	switch (expression->value->dataType)
// 	{
// 		case DataType::INT:			output += std::to_string(expression->value->immediate.i);	break;
// 		case DataType::BOOL:		output += expression->value->immediate.b ? "true" : "false";	break;
// 		case DataType::IDENTIFIER:	output += "identifier3";	break;
// 		case DataType::FUNCTION:	output += "function3";	break;
// 	}
//
// 	output += ";\n";
// 	return output;
// }

std::string Expression::convertPrint(Expression* expression)
{
	switch (expression->value->dataType)
	{
		case DataType::INT:			return "std::cout << " + std::to_string(expression->value->immediate.i) + " << \n;";	break;
		case DataType::BOOL:		return "std::cout << " + std::to_string(expression->value->immediate.b) + " << \n;";	break;
		case DataType::STRING:		return "std::cout << " + expression->value->str + " << \n;";							break;
		case DataType::IDENTIFIER:	break;
		case DataType::FUNCTION:	break;
	}

	return "";
}
