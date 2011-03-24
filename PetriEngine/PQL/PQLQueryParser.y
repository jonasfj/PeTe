%{
#include <stdio.h>
#include "PQLExpressions.h"
using namespace PetriEngine::PQL;

Condition* query;
extern int pqllex();
void pqlerror(const char *s) {printf("ERROR: %s\n", s);}
%}

%name-prefix "pql"

/* Possible data representation */
%union {
	PetriEngine::PQL::Expr* expr;
	PetriEngine::PQL::Condition* cond;
	std::string *string;
	int token;
}

/* Terminal type definition */
%token <string> ID INT
%token <token> LPAREN RPAREN
%token <token> AND OR NOT
%token <token> EQUAL NEQUAL LESS LESSEQUAL GREATER GREATEREQUAL
%token <token> PLUS MINUS MULTIPLY

/* Nonterminal type definition */
%type <expr> expr term factor
%type <cond> logic compare

/* Operator precedence, more possibly coming */

%start query

%%

query	: logic				{ query = $1; }
		| error				{ yyerrok; }
		;

logic	: logic AND compare	{ $$ = new AndCondition($1, $3); }
		| logic OR compare 	{ $$ = new OrCondition($1, $3); }
		| NOT logic			{ $$ = new NotCondition($2); }
		| compare			{ $$ = $1; }
		;

compare	: compare EQUAL expr		{ $$ = new EqualCondition($1, $3); }
		| compare NEQUAL expr		{ $$ = new NotEqualCondition($1, $3); }
		| compare LESS expr			{ $$ = new LessThanCondition($1, $3); }
		| compare LESSEQUAL expr 	{ $$ = new LessThanOrEqualCondition($1, $3); }
		| compare GREATER expr		{ $$ = new GreaterThanCondition($1, $3); }
		| compare GREATEREQUAL expr	{ $$ = new GreaterThanOrEqualCondition($1, $3); }
		| expr						{ $$ = $1; }
		;

expr	: expr PLUS term	{ $$ = new PlusExpr($1, $3); }
		| expr MINUS term	{ $$ = new SubtractExpr($1, $3); }
		| MINUS expr		{ $$ = new MinusExpr($2); }
		| term				{ $$ = $1; }
		;

term	: term MULTIPLY factor	{ $$ = new MultiplyExpr($1, $3); }
		| factor				{ $$ = $1; }
		;

factor	: LPAREN logic RPAREN	{ $$ = $2; }
		| INT			{ $$ = new LiteralExpr(atol($1->c_str())); delete $1; }
		| ID			{ $$ = new IdentifierExpr(*$1, $1.first_column); delete $1; }
		;
