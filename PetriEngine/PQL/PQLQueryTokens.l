%{
#include <string>
#include "PQLExpressions.h"
#include "PQLQueryParser.parser.hpp"
#define SAVE_TOKEN pqllval.string = new std::string(pqltext, pqlleng)
#define TOKEN(t) (pqllval.token = t)
extern "C" int pqlwrap(){}
extern PetriEngine::PQL::Condition* query;
extern int pqlparse();
%}
%option prefix="pql"

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
.							{printf("Unknown token %s!\n", pqltext); yyterminate();}

%%
namespace PetriEngine{ namespace PQL {
Condition* pqlParseQuery(const std::string& queryString) {
	//Load up input buffer in Flex
	YY_BUFFER_STATE buf = pql_scan_string(queryString.c_str());

	if(pqlparse() != 0)
		return NULL;

	//Delete the buffer
	pql_delete_buffer(buf);

	return query;
} 
}}
