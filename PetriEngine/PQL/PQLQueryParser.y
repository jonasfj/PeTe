%{
#include <stdio.h>
#include "PQL.h"
#include "Expressions.h"
using namespace PetriEngine::PQL;

Condition* query;
extern int pqlqlex();
void pqlqerror(const char *s) {printf("ERROR: %s\n", s);}
%}

%name-prefix "pqlq"
%expect 1

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

logic	: logic AND logic	{ $$ = new AndCondition($1, $3); }
		| logic OR logic 	{ $$ = new OrCondition($1, $3); }
		| NOT logic			{ $$ = new NotCondition($2); }
		| LPAREN logic RPAREN	{ $$ = $2; }
		| compare			{ $$ = $1; }
		;

compare	: expr EQUAL expr		{ $$ = new EqualCondition($1, $3); }
		| expr NEQUAL expr		{ $$ = new NotEqualCondition($1, $3); }
		| expr LESS expr			{ $$ = new LessThanCondition($1, $3); }
		| expr LESSEQUAL expr 	{ $$ = new LessThanOrEqualCondition($1, $3); }
		| expr GREATER expr		{ $$ = new GreaterThanCondition($1, $3); }
		| expr GREATEREQUAL expr	{ $$ = new GreaterThanOrEqualCondition($1, $3); }
		;

expr	: expr PLUS term	{ $$ = new PlusExpr($1, $3); }
		| expr MINUS term	{ $$ = new SubtractExpr($1, $3); }
		| MINUS expr		{ $$ = new MinusExpr($2); }
		| term				{ $$ = $1; }
		;

term	: term MULTIPLY factor	{ $$ = new MultiplyExpr($1, $3); }
		| factor				{ $$ = $1; }
		;

factor	: LPAREN expr RPAREN	{ $$ = $2; }
		| INT			{ $$ = new LiteralExpr(atol($1->c_str())); delete $1; }
		| ID			{ $$ = new IdentifierExpr(*$1, @1.first_column); delete $1; }
		;
