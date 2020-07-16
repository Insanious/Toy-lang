#include "Expression.h"
#include "Subroutine.h"

std::vector<std::string> builtinData = { "print" };
const std::vector<std::string> Expression::builtin = builtinData;

Expression::Expression(Subroutine* sub, ExprType exprType)
{
	this->sub = sub;
	this->exprType = exprType;
}

Expression::Expression(Subroutine* sub, ExprType exprType, BinOp op)
{
	this->exprType = exprType;
	this->op = op;
}

Expression* Expression::execute()
{
	switch (exprType)
	{
		case ExprType::DEFINITION:		executeDefinition();		break;
		case ExprType::ASSIGNMENT:		executeAssignment();		break;
		case ExprType::FUNCTIONCALL:	executeFunctionCall();		break;
		case ExprType::IFSTMT:			executeIfStatement();		break;
		case ExprType::IDENTIFIER:
		{
			if (sub->identifierExists(value->name))
			{
				Value* actualValue = sub->getIdentifierValue(value->name);
				delete value;
				value = actualValue;
			}

			return this;
		}
		case ExprType::BINOP:			return executeBinaryOperation();
		case ExprType::CONSTANT:		return this;
	}

	return nullptr;
}

void Expression::executeDefinition()
{
	std::cout << "executeDefinition()\n";

	Expression* expr = nullptr;

	if (expressions.size()) // variable is defined and equal to an expression
	{
		expr = expressions[0]->execute();
		if (value->dataType != expr->value->dataType)
			Subroutine::fatal("datatype mismatch: expected " + std::to_string((int)this->value->dataType) + " but got " + std::to_string((int)expr->value->dataType));

		switch (value->dataType) // assign the expression to the identifier
		{
			case DataType::INT:			value->immediate.i = expr->value->immediate.i;	break;
			case DataType::BOOL:		value->immediate.b = expr->value->immediate.b;	break;
			case DataType::IDENTIFIER:	std::cout << "executePrint() : IDENTIFIER";	break;
			case DataType::FUNCTION:	std::cout << "executePrint() : FUNCTION";	break;
		}
	}
	std::cout << "nani\n";

	sub->addNewIdentifier(value); // add symbol to subroutine
	// sub->output += convertDefinition(this->symbol, expr); // append to output
}

void Expression::executeAssignment()
{
	std::cout << "executeAssignment()\n";

	Value* currentValue = sub->getIdentifierValue(value->name);

	Expression* expr = expressions[0]->execute();
	if (currentValue->dataType != expr->value->dataType)
		Subroutine::fatal("datatype mismatch: expected " + std::to_string((int)currentValue->dataType) + " but got " + std::to_string((int)expr->value->dataType));

	switch (currentValue->dataType) // assign the expression to the identifier
	{
		case DataType::INT:			currentValue->immediate.i = expr->value->immediate.i;	break;
		case DataType::BOOL:		currentValue->immediate.b = expr->value->immediate.b;	break;
		case DataType::IDENTIFIER:	std::cout << "executeAssignment() : IDENTIFIER";	break;
		case DataType::FUNCTION:	std::cout << "executeAssignment() : FUNCTION";	break;
	}
	// delete currentValue; // delete old and ...
	// currentValue = expr; // .. replace with new

	// sub->output += convertAssignment(symbol, expr); // append to output
}

Expression* Expression::executeFunctionCall()
{
	// std::cout << "executeFunctionCall()\n";

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

Expression* Expression::executeBinaryOperation()
{
	switch (op)
	{
		case BinOp::EQUALS: return binOpEquals();
	}

	return nullptr;
}

void Expression::executeIfStatement()
{
	std::cout << "executeIfStatement()\n";

	Expression* expr = expressions[0]->execute();

	if (expr->exprType != ExprType::CONSTANT)
		Subroutine::fatal("expected ExprType::CONSTANT but got " + std::to_string((int)expr->exprType));
	if (expr->value->dataType != DataType::BOOL)
		Subroutine::fatal("expected DataType::BOOL but got " + std::to_string((int)expr->value->dataType));

	if (expr->value->immediate.b)
	{
		int size = expressions.size();
		for (int i = 1; i < size; i++)
			expressions[i]->execute();
	}
}

void Expression::executePrint()
{
	std::cout << "executePrint()\n";

	Expression* expr = expressions[0]->execute();

	switch (expr->value->dataType)
	{
		case DataType::INT:			std::cout << std::to_string(expr->value->immediate.i) << '\n';	break;
		case DataType::BOOL:		std::cout << (expr->value->immediate.b ? "true" : "false") << '\n';	break;
		case DataType::IDENTIFIER:	std::cout << "executePrint() : IDENTIFIER";	break;
		case DataType::FUNCTION:	std::cout << "executePrint() : FUNCTION";	break;
	}
}

Expression* Expression::binOpEquals()
{
	std::cout << "binOpEquals()\n";
	Expression* left = expressions[0]->execute();
	Expression* right = expressions[1]->execute();

	if (left->value->dataType != right->value->dataType)
		Subroutine::fatal("expected " + std::to_string((int)left->value->dataType) + " but got " + std::to_string((int)right->value->dataType));

	Expression* expr = new Expression(sub, ExprType::CONSTANT);
	expr->value = new Value(DataType::BOOL);

	switch (left->value->dataType)
	{
		case DataType::INT:			expr->value->immediate.b = (left->value->immediate.i == right->value->immediate.i); break;
		case DataType::BOOL:		expr->value->immediate.b = (left->value->immediate.b == right->value->immediate.b); break;
		case DataType::IDENTIFIER:	Subroutine::fatal("binOpEquals: left->dataType == IDENTIFIER"); break;
		case DataType::FUNCTION:	Subroutine::fatal("binOpEquals: left->dataType == FUNCTION"); break;
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
		case DataType::INT:			return "std::cout << " + std::to_string(expression->value->immediate.i) + " << \n;"; break;
		case DataType::BOOL:		return "std::cout << " + std::to_string(expression->value->immediate.b) + " << \n;"; break;
		case DataType::IDENTIFIER:	break;
		case DataType::FUNCTION:	break;
	}

	return "";
}
