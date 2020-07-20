#include "Expression.h"
#include "Subroutine.h"

std::vector<std::string> builtinData = { "print" };
const std::vector<std::string> Expression::builtin = builtinData;

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

Expression::Expression(Subroutine* sub, ExprType exprType)
{
	this->sub = sub;
	this->exprType = exprType;
}

Expression::Expression(Subroutine* sub, ExprType exprType, BinOp op)
{
	this->sub = sub;
	this->exprType = exprType;
	this->op = op;
}

Expression* Expression::execute()
{
	log_executions("execute() " + std::to_string(int(exprType)));

	switch (exprType)
	{
		case ExprType::DEFINITION:		executeDefinition();		break;
		case ExprType::ASSIGNMENT:		executeAssignment();		break;
		case ExprType::FUNCTIONCALL:	executeFunctionCall();		break;
		case ExprType::IFSTMT:			executeIfStatement();		break;
		case ExprType::FORLOOP:			executeForLoop();			break;
		case ExprType::IDENTIFIER:		value = sub->getIdentifierValue(value->name);	return this;
		case ExprType::BINOP:			return executeBinOp();
		case ExprType::CONSTANT:		return this;
	}

	return nullptr;
}

void Expression::executeDefinition()
{
	log_executions("executeDefinition()");

	Expression* expr = nullptr;

	if (expressions.size()) // variable is defined and equal to an expression
	{
		expr = expressions[0]->execute();
		if (value->dataType != expr->value->dataType)
			Subroutine::fatal("datatype mismatch: expected " + std::to_string((int)value->dataType) + " but got " + std::to_string((int)expr->value->dataType));

		switch (value->dataType) // assign the expression to the identifier
		{
			case DataType::INT:			value->immediate.i = expr->value->immediate.i;	break;
			case DataType::BOOL:		value->immediate.b = expr->value->immediate.b;	break;
			case DataType::STRING:		value->str = expr->value->str;					break;
			case DataType::IDENTIFIER:	std::cout << "executePrint() : IDENTIFIER";	break;
			case DataType::FUNCTION:	std::cout << "executePrint() : FUNCTION";	break;
		}
	}

	sub->addIdentifier(value); // add symbol to subroutine
	// sub->output += convertDefinition(this->symbol, expr); // append to output
}

void Expression::executeAssignment()
{
	log_executions("executeAssignment()");

	Value* currentValue = sub->getIdentifierValue(value->name);

	Expression* expr = expressions[0]->execute();
	if (currentValue->dataType != expr->value->dataType)
		Subroutine::fatal("datatype mismatch: expected " + std::to_string((int)currentValue->dataType) + " but got " + std::to_string((int)expr->value->dataType));

	switch (currentValue->dataType) // assign the expression to the identifier
	{
		case DataType::INT:			currentValue->immediate.i = expr->value->immediate.i;	break;
		case DataType::BOOL:		currentValue->immediate.b = expr->value->immediate.b;	break;
		case DataType::STRING:		currentValue->str = expr->value->str;					break;
		case DataType::IDENTIFIER:	std::cout << "executeAssignment() : IDENTIFIER";		break;
		case DataType::FUNCTION:	std::cout << "executeAssignment() : FUNCTION";			break;
	}
	// delete currentValue; // delete old and ...
	// currentValue = expr; // .. replace with new

	// sub->output += convertAssignment(symbol, expr); // append to output
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
			// sub->output += convertPrint(expressions[0]->execute()); // reduce expression to single-value expression
		}
	}

	return result;
}

Expression* Expression::executeBinOp()
{
	log_executions("executeBinOp()");
	switch (op)
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
		Subroutine::fatal("expected ExprType::CONSTANT but got " + std::to_string((int)expr->exprType));
	if (expr->value->dataType != DataType::BOOL)
		Subroutine::fatal("expected DataType::BOOL but got " + std::to_string((int)expr->value->dataType));

	if (expr->value->immediate.b)
	{
		Subroutine scope = Subroutine(sub);

		this->setScope(&scope); // recursively set the scope of all subexpressions
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
			Subroutine::fatal("expected DataType::INT but got " + std::to_string((int)condition->value->dataType));

		Subroutine scope = Subroutine(sub);
		this->setScope(&scope);
		int size = block.size();
		for (int i = 0; i < condition->value->immediate.i; i++)
		{
			for (int j = 0; j < size; j++)
				block[j]->execute();

			scope.clearIdentifiers();
		}
	}
}

void Expression::executePrint()
{
	log_executions("executePrint()");

	Expression* expr = expressions[0]->execute();

	switch (expr->value->dataType)
	{
		case DataType::INT:			std::cout << std::to_string(expr->value->immediate.i) << '\n';		break;
		case DataType::BOOL:		std::cout << (expr->value->immediate.b ? "true" : "false") << '\n';	break;
		case DataType::STRING:		std::cout << expr->value->str << '\n';								break;
		case DataType::IDENTIFIER:	std::cout << "executePrint() : IDENTIFIER";							break;
		case DataType::FUNCTION:	std::cout << "executePrint() : FUNCTION";							break;
	}
}

void Expression::setScope(Subroutine* scope)
{
	for (auto statement : block)
	{
		statement->sub = scope;
		statement->setScope(scope);
	}
}

Expression* Expression::binOpEquals()
{
	log_binOps("binOpEquals()");

	Value* left = expressions[0]->execute()->value;
	Value* right = expressions[1]->execute()->value;

	if (left->dataType != right->dataType)
		Subroutine::fatal("expected " + std::to_string((int)left->dataType) + " but got " + std::to_string((int)right->dataType));

	Expression* expr = new Expression(sub, ExprType::CONSTANT);
	expr->value = new Value(DataType::BOOL);

	switch (left->dataType)
	{
		case DataType::INT:			expr->value->immediate.b = (left->immediate.i == right->immediate.i);	break;
		case DataType::BOOL:		expr->value->immediate.b = (left->immediate.b == right->immediate.b);	break;
		case DataType::STRING:		expr->value->immediate.b = (left->str == right->str);					break;
		case DataType::IDENTIFIER:	Subroutine::fatal("binOpEquals: left->dataType == IDENTIFIER");			break;
		case DataType::FUNCTION:	Subroutine::fatal("binOpEquals: left->dataType == FUNCTION");			break;
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
