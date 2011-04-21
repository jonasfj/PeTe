%{
#include <stdio.h>
#include "PQL.h"
#include "Expressions.h"
using namespace PetriEngine::PQL;

AssignmentExpression* assignment;
extern int pqlalex();
void pqlaerror(const char *s) {printf("ERROR: %s\n", s);}
%}

%name-prefix "pqla"
%expect 2

/* Possible data representation */
%union {
	PetriEngine::PQL::Expr* expr;
	PetriEngine::PQL::AssignmentExpression* assExpr;
	std::string *string;
	int token;
}

/* Terminal type definition */
%token <string> ID INT
%token <token> LPAREN RPAREN ASSIGN SEMI
%token <token> AND OR NOT
%token <token> EQUAL NEQUAL LESS LESSEQUAL GREATER GREATEREQUAL
%token <token> PLUS MINUS MULTIPLY

/* Nonterminal type definition */
%type <expr> expr term factor
%type <assExpr> assignment

/* Operator precedence, more possibly coming */

%start root

%%

root 	: assignment			{ assignment = (AssignmentExpression*)$1; }
		| error					{ assignment = NULL; yyerrok; }
		;

assignment	: ID ASSIGN expr SEMI assignment 	{ ((AssignmentExpression*)$5)->prepend(*$1, $3); delete $1;
												  $$ = ((AssignmentExpression*)$5)}
			| ID ASSIGN expr SEMI				{ AssignmentExpression* a = new AssignmentExpression();
												  a->prepend(*$1, $3); delete $1;
												  $$ = a;}
			| ID ASSIGN expr					{ AssignmentExpression* a = new AssignmentExpression();
												  a->prepend(*$1, $3); delete $1;
												  $$ = a;}
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
