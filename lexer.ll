%top{
#include "grammar.tab.hh"
#include <stdio.h>
#include <string>
#include "globals.h"

#define YY_DECL yy::parser::symbol_type yylex()

int lineNumber = 1;

void log_lexer(std::string message)
{
	if (debug_lex)
		std::cout << "LEX:\t\t " << message << '\n';
}

}
%option noyywrap nounput batch noinput
%%

 /* Control-flow */
if						{ log_lexer(yytext); return yy::parser::make_IF(yytext); }
then					{ log_lexer(yytext); return yy::parser::make_THEN(yytext); }
elseif					{ log_lexer(yytext); return yy::parser::make_ELSEIF(yytext); }
else					{ log_lexer(yytext); return yy::parser::make_ELSE(yytext); }

 /* Looping */
for						{ log_lexer(yytext); return yy::parser::make_FOR(yytext); }
while					{ log_lexer(yytext); return yy::parser::make_WHILE(yytext); }

 /* Binary operations */
\+						{ log_lexer(yytext); return yy::parser::make_PLUS(yytext); }
\-						{ log_lexer(yytext); return yy::parser::make_MINUS(yytext); }
\*						{ log_lexer(yytext); return yy::parser::make_MUL(yytext); }
\/						{ log_lexer(yytext); return yy::parser::make_DIV(yytext); }
\^						{ log_lexer(yytext); return yy::parser::make_POWER_OF(yytext); }
\%						{ log_lexer(yytext); return yy::parser::make_MOD(yytext); }
\=\=					{ log_lexer(yytext); return yy::parser::make_EQUALS(yytext); }
\!\=					{ log_lexer(yytext); return yy::parser::make_NOT_EQUALS(yytext); }
\-\>					{ log_lexer(yytext); return yy::parser::make_LARROW(yytext); }
\<						{ log_lexer(yytext); return yy::parser::make_LESS(yytext); }
\>						{ log_lexer(yytext); return yy::parser::make_MORE(yytext); }
\<\=					{ log_lexer(yytext); return yy::parser::make_LESS_OR_EQUAL(yytext); }
\>\=					{ log_lexer(yytext); return yy::parser::make_MORE_OR_EQUAL(yytext); }

 /* Unary operations */
not						{ log_lexer(yytext); return yy::parser::make_NOT(yytext); }
and						{ log_lexer(yytext); return yy::parser::make_AND(yytext); }
or						{ log_lexer(yytext); return yy::parser::make_OR(yytext); }

 /* Functions */
function				{ log_lexer(yytext); return yy::parser::make_FUNCTION(yytext); }
break					{ log_lexer(yytext); return yy::parser::make_BREAK(yytext); }
return					{ log_lexer(yytext); return yy::parser::make_RETURN(yytext); }

 /* Arguments */

 /* Values */
false					{ log_lexer(yytext); return yy::parser::make_FALSE(false); }
true					{ log_lexer(yytext); return yy::parser::make_TRUE(true); }
[0-9]+\.[0-9]+	 		{ log_lexer(yytext); return yy::parser::make_FLOAT(yytext); }
[0-9]+					{ log_lexer(yytext); return yy::parser::make_NUMBER(std::stoi(yytext)); }

 /* Types */
"none"					{ log_lexer(yytext); return yy::parser::make_NONE(yytext); }
"int"					{ log_lexer(yytext); return yy::parser::make_INT(yytext); }
"bool"					{ log_lexer(yytext); return yy::parser::make_BOOL(yytext); }
"string"				{ log_lexer(yytext); return yy::parser::make_STRING(yytext); }

\"[^\*\"]*\"			{ log_lexer(yytext); std::string str = yytext; return yy::parser::make_TEXT(str.substr(1, yyleng - 2)); }
[a-zA-Z_][a-zA-Z0-9_]*	{ log_lexer(yytext); return yy::parser::make_ID(yytext); }

 /* Single-character tokens */
\=						{ log_lexer(yytext); return yy::parser::make_ASSIGN(yytext); }
\.						{ log_lexer(yytext); return yy::parser::make_DOT(yytext); }
\:						{ log_lexer(yytext); return yy::parser::make_COLON(yytext); }
\;						{ log_lexer(yytext); return yy::parser::make_SEMICOLON(yytext); }
\,						{ log_lexer(yytext); return yy::parser::make_COMMA(yytext); }
\(						{ log_lexer(yytext); return yy::parser::make_LROUND(yytext); }
\)						{ log_lexer(yytext); return yy::parser::make_RROUND(yytext); }
\[						{ log_lexer(yytext); return yy::parser::make_LSQUARE(yytext); }
\]						{ log_lexer(yytext); return yy::parser::make_RSQUARE(yytext); }
\{						{ log_lexer(yytext); return yy::parser::make_LCURLY(yytext); }
\}						{ log_lexer(yytext); return yy::parser::make_RCURLY(yytext); }

 /* Whitespace */
" "+					{}
\t+						{}
\n+						{ lineNumber++; log_lexer("\"\\n\"+"); /*return yy::parser::make_NEWLINE(yytext);*/ }
\/\/.*\n+				{}

 /* Misc */
<<EOF>>					{ return yy::parser::make_EXIT(); }

%%
