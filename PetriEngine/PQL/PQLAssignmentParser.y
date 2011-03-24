%{
#include <stdio.h>
#include "CTLExprs.h"
using namespace PetriEngine::PQL;

AssignmentExpression* assignment;
extern int pqllex();
void pqlerror(const char *s) {printf("ERROR: %s\n", s);}
%}

%name-prefix "pql"

/* Possible data representation */
%union {
	PetriEngine::PQL::Expr* expr;
	PetriEngine::PQL::AssignmentExpression* assignment;
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
%type <expr> logic expr term factor compare

/* Operator precedence, more possibly coming */

%start assignment

%%

assignment	: ID ASSIGN expr SEMI assignment 	{ assignment = assignment.add($1, $3); }
			| ID ASSIGN expr SEMI				{ assignment = new Assignment();
												  assignment.add($1, $3);}
			| error								{ yyerrok; }
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
		| ID			{ $$ = new IdentifierExpr(*$1); delete $1; }
		;
