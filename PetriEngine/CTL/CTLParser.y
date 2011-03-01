%{
#include <stdio.h>
#include "CTLExprs.h"
using namespace PetriEngine::CTL;

CTLExpr* query;
extern int ctllex();
void ctlerror(const char *s) {printf("ERROR: %s\n", s);}
%}

/* Possible data representation */
%union {
	PetriEngine::CTL::CTLExpr* expr;
	std::string *string;
	int token;
}

/* Terminal type definition */
%token <string> PLACE INT
%token <token> LPAREN RPAREN
%token <token> AND OR NOT
%token <token> EQUAL NEQUAL LESS LESSEQUAL GREATER GREATEREQUAL
%token <token> PLUS MINUS MULTIPLY DIVIDE

/* Nonterminal type definition */
%type <expr> logic expr term factor compare

/* Operator precedence, more possibly coming */

%start query

%%

query	: logic				{ query = $1; }
		;

logic	: logic AND compare	{ $$ = new AndExpr($1, $3); }
		| logic OR compare 	{ $$ = new OrExpr($1, $3); }
		| NOT logic			{ $$ = new NotExpr($2); }
		| compare			{ $$ = $1; }
		;

compare	: compare EQUAL expr	{ $$ = new EqualExpr($1, $3); }
		| compare NEQUAL expr	{ $$ = new NEqualExpr($1, $3); }
		| compare LESS expr		{ $$ = new LessExpr($1, $3); }
		| compare LESSEQUAL expr { $$ = new LessEqualExpr($1, $3); }
		| compare GREATER expr	{ $$ = new GreaterExpr($1, $3); }
		| compare GREATEREQUAL expr	{ $$ = new GreaterEqualExpr($1, $3); }
		| expr
		;

expr	: expr PLUS term	{ $$ = new PlusExpr($1, $3); }
		| expr MINUS term	{ $$ = new MinusExpr($1, $3); }
		| term				{ $$ = $1; }
		;

term	: term MULTIPLY factor	{ $$ = new MultiplyExpr($1, $3); }
		| term DIVIDE factor	{ $$ = new DivideExpr($1, $3); }
		| factor				{ $$ = $1; }
		;

factor	: LPAREN expr RPAREN	{ $$ = $2; }
		| INT			{ $$ = new LiteralExpr(atol($1->c_str())); delete $1; }
		| PLACE			{ $$ = new PlaceExpr(*$1); delete $1; }
		;
