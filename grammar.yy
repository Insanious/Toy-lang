%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%define api.token.constructor

%code
{
	#define YY_DECL yy::parser::symbol_type yylex()
	YY_DECL;

	void log_grammar(std::string message)
	{
		if (debug_grammar)
			std::cout << "GRAMMAR:\t " << message << '\n';
	}

	Subroutine* global;
}

%code requires
{
	#include "globals.h"
	#include "Enums.h"
	#include "Subroutine.h"
	#include "Expression.h"
	#include <string>

}

%token EXIT 0 "end of file"

/* Control-flow */
%token <std::string> IF
%token <std::string> THEN
%token <std::string> ELSEIF
%token <std::string> ELSE

/* Looping */
%token <std::string> FOR
%token <std::string> WHILE

/* Binary operations */
%token <std::string> PLUS
%token <std::string> MINUS
%token <std::string> MUL
%token <std::string> DIV
%token <std::string> POWER_OF
%token <std::string> MOD
%token <std::string> EQUALS
%token <std::string> NOT_EQUALS
%token <std::string> LESS
%token <std::string> MORE
%token <std::string> LESS_OR_EQUAL
%token <std::string> MORE_OR_EQUAL

/* Functions */
%token <std::string> RETURN
%token <std::string> BREAK
%token <std::string> FUNCTION


/* Unary operations */
%token <std::string> NOT
%token <std::string> AND
%token <std::string> OR

/* Types */
%token <std::string> NONE
%token <std::string> INT
%token <std::string> BOOL
%token <std::string> STRING
%token <std::string> FLOAT
%token <std::string> ID

/* Values */
%token <std::string> TEXT
%token <int> NUMBER
%token <bool> TRUE
%token <bool> FALSE

/* Single-character tokens */
%token <std::string> ASSIGN
%token <std::string> DOT
%token <std::string> COLON
%token <std::string> SEMICOLON
%token <std::string> COMMA
%token <std::string> LARROW
%token <std::string> LROUND
%token <std::string> RROUND
%token <std::string> LSQUARE
%token <std::string> RSQUARE
%token <std::string> LCURLY
%token <std::string> RCURLY

%type <Subroutine*> subroutine

%type <std::vector<Expression*>> stmts
%type <std::vector<Expression*>> block

%type <Expression*> stmt
%type <Expression*> vardef
%type <Expression*> funcdef
%type <Expression*> functioncall
%type <Expression*> assignment
%type <Expression*> ifstmt
%type <Expression*> forloop

%type <Expression*> expr
%type <Expression*> op
%type <Expression*> op_1
%type <Expression*> op_last

%type <DataType> type


%%

program
	: subroutine					{ log_grammar("program:subroutine"); }

subroutine
	: stmts							{ log_grammar("subroutine:stmts"); $$ = global; $$->expressions = $1; }

stmts
	: stmt 							{ log_grammar("stmts:stmt"); $$.push_back($1); }
	| stmts stmt					{ log_grammar("stmts:stmts stmt"); $$ = $1; $$.push_back($2); }

stmt
	: vardef SEMICOLON				{ log_grammar("stmt:vardef"); $$ = $1; }
	| assignment SEMICOLON			{ log_grammar("stmt:assignment"); $$ = $1; }
	| functioncall SEMICOLON		{ log_grammar("stmt:functioncall"); $$ = $1; }
	| funcdef						{ log_grammar("stmt:functiondefinition"); $$ = $1; }
	| ifstmt						{ log_grammar("stmt:ifstmt"); $$ = $1; }
	| forloop						{ log_grammar("stmt:forloop"); $$ = $1; }

ifstmt
	: IF LROUND expr RROUND stmt	{ log_grammar("ifstmt:IF expr"); $$ = new Expression(global, ExprType::IFSTMT); $$->expressions.push_back($3); $$->block.push_back($5); }
	| IF LROUND expr RROUND block	{ log_grammar("ifstmt:IF expr"); $$ = new Expression(global, ExprType::IFSTMT); $$->expressions.push_back($3); $$->block = $5; }

forloop
	: stmt MUL expr SEMICOLON		{ log_grammar("forloop:bracketloop"); $$ = new Expression(global, ExprType::FORLOOP); $$->block.push_back($1); $$->expressions.push_back($3); }
	| block MUL expr SEMICOLON		{ log_grammar("forloop:bracketloop"); $$ = new Expression(global, ExprType::FORLOOP); $$->block = $1; $$->expressions.push_back($3); }

block
	: LCURLY stmts RCURLY			{ log_grammar("block:"); $$ = $2; }

funcdef
	: type ID LROUND RROUND block	{ log_grammar("funcdef:"); $$ = new Expression(global, ExprType::FUNCTIONDEFINITION); $$->value = new Value($1); $$->functionName = $2; $$->block = $5; }
	//| type ID LROUND expr RROUND block	{ log_grammar("funcdef:"); $$ = new Expression(global, ExprType::FUNCTIONDEFINITION); $$->value = new Value($1); $$->functionName = $2; $$$$->block = $6; }

vardef
	: type ID ASSIGN expr			{ log_grammar("vardef:expr"); $$ = new Expression(global, ExprType::DEFINITION); $$->value = new Value($1); $$->value->name = $2; $$->expressions.push_back($4); }
	| type ID						{ log_grammar("vardef:"); $$ = new Expression(global, ExprType::DEFINITION); $$->value = new Value($1); $$->value->name = $2; }

functioncall
	: ID LROUND RROUND				{ log_grammar("functioncall:"); $$ = new Expression(global, ExprType::FUNCTIONCALL); $$->functionName = $1; }
	| ID LROUND expr RROUND			{ log_grammar("functioncall:expr"); $$ = new Expression(global, ExprType::FUNCTIONCALL); $$->functionName = $1; $$->expressions.push_back($3); }

assignment
	: ID ASSIGN expr				{ log_grammar("assignment:ID = expr"); $$ = new Expression(global, ExprType::ASSIGNMENT); $$->value = new Value(DataType::IDENTIFIER); $$->value->name = $1; $$->expressions.push_back($3); }

type
	: INT							{ log_grammar("type:INT"); $$ = DataType::INT; }
	| BOOL							{ log_grammar("type:BOOL"); $$ = DataType::BOOL; }
	| STRING						{ log_grammar("type:STRING"); $$ = DataType::STRING; }

expr
	: op							{ log_grammar("expr:op"); $$ = $1; }

op
	: op_1							{ log_grammar("op:op_1"); $$ = $1; }
	| op EQUALS op_1				{ log_grammar("op:op == op_1"); $$ = new Expression(global, ExprType::BINOP, BinOp::EQUALS); $$->expressions.push_back($1); $$->expressions.push_back($3); }

op_1
	: op_last						{ log_grammar("op_1:op_last"); $$ = $1; }

op_last
	: NUMBER						{ log_grammar("expr:NUMBER");	$$ = new Expression(global, ExprType::CONSTANT);	$$->value = new Value(DataType::INT);			$$->value->immediate.i = $1; }
	| TRUE							{ log_grammar("expr:TRUE");		$$ = new Expression(global, ExprType::CONSTANT);	$$->value = new Value(DataType::BOOL);			$$->value->immediate.b = true; }
	| FALSE							{ log_grammar("expr:FALSE");	$$ = new Expression(global, ExprType::CONSTANT);	$$->value = new Value(DataType::BOOL);			$$->value->immediate.b = false; }
	| TEXT							{ log_grammar("expr:TEXT");		$$ = new Expression(global, ExprType::CONSTANT);	$$->value = new Value(DataType::STRING);		$$->value->str = $1; }
	| ID							{ log_grammar("expr:ID");		$$ = new Expression(global, ExprType::IDENTIFIER);	$$->value = new Value(DataType::IDENTIFIER);	$$->value->name = $1; }
