%{
#include <stdio.h>
#include "PQL.h"
#include "Expressions.h"
using namespace PetriEngine::PQL;

Condition* sumoQuery;
extern int sumolex();
extern char* sumotext;
void sumoerror(const char *s) {printf("ERROR: %s: %s\n", s, sumotext);}
%}

%name-prefix "sumo"

/* Data representation */
%union {
	PetriEngine::PQL::Expr* expr;
	PetriEngine::PQL::Condition* cond;
	std::string* string;
	int token;
}

/* Terminal type definition */
%token <string> ID INT
%token <token> LPAREN RPAREN LBRACKET RBRACKET
%token <token> AND OR NOT REACHABLE INVARIANT
%token <token> CONTAINS EQUALS STAR VALUE EQUAL

/* Nonterminal type definition */
%type <cond> subformula atomic
%type <expr> idexpr expr


/* Lets begin */

%start root

%%

root	: ID EQUAL REACHABLE subformula		{ sumoQuery = $4; }
		| ID EQUAL INVARIANT subformula		{ sumoQuery = NULL; /*TODO: Fix later*/ }
		;

subformula : subformula AND subformula		{ $$ = new AndCondition($1, $3); }
		   | subformula OR subformula		{ $$ = new OrCondition($1, $3); }
		   | NOT subformula					{ $$ = new NotCondition($2); }
		   | LPAREN subformula RPAREN		{ $$ = $2; }
		   | atomic							{ $$ = $1; }
		   ;

atomic : idexpr CONTAINS LBRACKET expr RBRACKET { $$ = new GreaterThanOrEqualCondition($1, $4); }
	   | idexpr EQUALS LBRACKET expr RBRACKET { $$ = new EqualCondition($1, $4); }
	   | idexpr EQUALS LBRACKET RBRACKET { $$ = new EqualCondition($1, new LiteralExpr(0)); }
	   ;

idexpr : ID		{ $$ = new IdentifierExpr(*$1, @1.first_column); delete $1; }
	   ;

expr : INT STAR VALUE	{ $$ = new LiteralExpr(atol($1->c_str())); delete $1; }
	 ;
