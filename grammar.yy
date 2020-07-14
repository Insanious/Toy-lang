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

	Subroutine* currentSub;
}

%code requires
{
	#include "globals.h"
	#include "Enums.h"
	#include "Subroutine.h"
	#include "Expression.h"
	#include "Symbol.h"
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

/* Unary operations */
%token <std::string> NOT
%token <std::string> AND
%token <std::string> OR

/* Functions */
%token <std::string> FUNCTION
%token <std::string> BREAK
%token <std::string> RETURN

/* Types */
%token <std::string> INT

/* Values */
%token <std::string> ID
%token <std::string> STRING
%token <int> NUMBER
%token <std::string> BOOL
%token <bool> TRUE
%token <bool> FALSE
%token <std::string> FLOAT

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

%type <Expression*> stmt
%type <Expression*> vardef
%type <Expression*> functioncall
%type <Expression*> assignment
%type <Expression*> expr

%type <DataType> type


%%

program
	: subroutine					{ log_grammar("program:subroutine"); }

subroutine
	: stmts							{ log_grammar("subroutine:stmts"); $$ = currentSub; $$->expressions = $1; }

stmts
	: stmt							{ log_grammar("stmts:stmt"); $$.push_back($1); }
	| stmts stmt					{ log_grammar("stmts:stmts stmt"); $$ = $1; $$.push_back($2); }

stmt
	: vardef 						{ log_grammar("stmt:vardef"); $$ = $1; }
	| assignment					{ log_grammar("stmt:assignment"); $$ = $1; }
	| functioncall					{ log_grammar("stmt:functioncall"); $$ = $1; }

vardef
	: type ID ASSIGN expr SEMICOLON	{ log_grammar("vardef:expr"); $$ = new Expression(currentSub, ExprType::DEFINITION, new Symbol($1, $2)); $$->expressions.push_back($4); }
	| type ID SEMICOLON				{ log_grammar("vardef:"); $$ = new Expression(currentSub, ExprType::DEFINITION, new Symbol($1, $2)); }

functioncall
	: ID LROUND expr RROUND			{ log_grammar("functioncall:expr"); $$ = new Expression(currentSub, ExprType::FUNCTIONCALL, new Symbol(DataType::FUNCTION, $1)); $$->expressions.push_back($3); }
// 	| builtin LROUND expr RROUND	{ log_grammar("functioncall:expr"); $$ = new Expression(currentSub, ExprType::FUNCTIONCALL, new Symbol($1)); $$->expressions.push_back($3); }
//
// builtin
// 	: PRINT

type
	: INT							{ log_grammar("type:INT"); $$ = DataType::INT; }
	| BOOL							{ log_grammar("type:BOOL"); $$ = DataType::BOOL; }

assignment
	: ID ASSIGN expr SEMICOLON		{ log_grammar("assignment:ID = expr"); $$ = new Expression(currentSub, ExprType::ASSIGNMENT); $$->strValue = $1; $$->expressions.push_back($3); }

expr
	: NUMBER	{ log_grammar("expr:NUMBER"); $$ = new Expression(currentSub, ExprType::CONSTANT); $$->dataType = DataType::INT; $$->value.i = $1; }
	| TRUE		{ log_grammar("expr:TRUE"); $$ = new Expression(currentSub, ExprType::CONSTANT); $$->dataType = DataType::BOOL; $$->value.b = $1; }
	| FALSE		{ log_grammar("expr:FALSE"); $$ = new Expression(currentSub, ExprType::CONSTANT); $$->dataType = DataType::BOOL; $$->value.b = $1; }
	| ID		{ log_grammar("expr:ID"); $$ = new Expression(currentSub, ExprType::IDENTIFIER); $$->strValue = $1; }
