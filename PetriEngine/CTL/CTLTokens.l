%{
#include <string>
#include "CTLExprs.h"
#include "CTLParser.parser.hpp"
#define SAVE_TOKEN ctllval.string = new std::string(ctltext, ctlleng)
#define TOKEN(t) (ctllval.token = t)
extern "C" int ctlwrap(){}
extern PetriEngine::CTL::CTLExpr* query;
extern int ctlparse();
%}
%option prefix="ctl"

digit         [0-9]
letter        [a-zA-Z]

%%

[ \t\n\r]					;
"and"						{return TOKEN(AND);}
"AND"						{return TOKEN(AND);}
"or"						{return TOKEN(OR);}
"OR"						{return TOKEN(OR);}
{letter}({letter}|{digit})*	{SAVE_TOKEN; return PLACE;}
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
"/"							{return TOKEN(DIVIDE);}
.							{printf("Unknown token %s!\n", ctltext); yyterminate();}

%%

PetriEngine::CTL::CTLExpr* ctlParseString(const std::string& queryString) {
	//Load up input buffer in Flex
	YY_BUFFER_STATE buf = ctl_scan_string(queryString.c_str());

	if(ctlparse() == 0)
		return null;

	//Delete the buffer
	ctl_delete_buffer(buf);

	return query;
} 
