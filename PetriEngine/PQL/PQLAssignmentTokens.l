%{
#include <string>
#include "PQL.h"
#include "PQLAssignmentParser.parser.hpp"
#define SAVE_TOKEN pqlalval.string = new std::string(pqlatext, pqlaleng)
#define TOKEN(t) (pqlalval.token = t)
extern "C" int pqlawrap(){return 1;}
extern PetriEngine::PQL::AssignmentExpression* assignment;
extern int pqlaparse();
%}
%option prefix="pqla"

digit         [0-9]
letter        [a-zA-Z]

%%

[ \t\n\r]					;
"and"						{return TOKEN(AND);}
"AND"						{return TOKEN(AND);}
"or"						{return TOKEN(OR);}
"OR"						{return TOKEN(OR);}
{letter}({letter}|{digit})*	{SAVE_TOKEN; return ID;}
{digit}+					{SAVE_TOKEN; return INT;}
":="						{return TOKEN(ASSIGN);}
"&&"						{return TOKEN(AND);}
"||"						{return TOKEN(OR);}
"=="						{return TOKEN(EQUAL);}
"!="						{return TOKEN(NEQUAL);}
"<"							{return TOKEN(LESS);}
"<="						{return TOKEN(LESSEQUAL);}
">"							{return TOKEN(GREATER);}
">="						{return TOKEN(GREATEREQUAL);}
"("							{return TOKEN(LPAREN);}
")"							{return TOKEN(RPAREN);}
"+"							{return TOKEN(PLUS);}
"-"							{return TOKEN(MINUS);}
"*"							{return TOKEN(MULTIPLY);}
";"							{return TOKEN(SEMI);}
.							{printf("Unknown token %s!\n", pqlatext); yyterminate();}

%%

namespace PetriEngine { namespace PQL {
AssignmentExpression* ParseAssignment(const std::string& assignmentString) {
	assignment = NULL;

	//Load up input buffer in Flex
	YY_BUFFER_STATE buf = pqla_scan_string(assignmentString.c_str());

	if(pqlaparse() != 0)
		return NULL;

	//Delete the buffer
	pqla_delete_buffer(buf);

	return assignment;
} 
}}
