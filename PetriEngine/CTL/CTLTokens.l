%{
#include <string>
#include "CTLExprs.h"
#include "CTLParser.hpp"
#define SAVE_TOKEN ctllval.string = new std::string(ctltext, ctlleng)
#define TOKEN(t) (ctllval.token = t)
extern "C" int ctlwrap(){}
%}

digit         [0-9]
letter        [a-zA-Z]

%%

[ \t\n\r]					;
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
