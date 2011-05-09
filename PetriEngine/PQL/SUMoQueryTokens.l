%{
#include <string>
#include "PQL.h"
#include "PQLParser.h"
#include "SUMoQueryParser.parser.hpp"
#define SAVE_TOKEN sumolval.string = new std::string(sumotext, sumoleng)
#define TOKEN(t) (sumolval.token = t)
extern "C" int sumowrap(){return 1;}
extern PetriEngine::PQL::Condition* sumoQuery;
extern std::string sumoName;
extern bool isInvariant;
extern int sumoparse();
%}
%option prefix="sumo"
%option nounput

digit			[0-9]
letter			[_a-zA-Z]
id				{letter}({letter}|{digit})*

%%

[ \t\n\r]					;
"and"						{return TOKEN(AND);}
"AND"						{return TOKEN(AND);}
"or"						{return TOKEN(OR);}
"OR"						{return TOKEN(OR);}
"not"						{return TOKEN(NOT);}
"NOT"						{return TOKEN(NOT);}
"reachable"					{return TOKEN(REACHABLE);}
"invariant"					{return TOKEN(INVARIANT);}
"contains"					{return TOKEN(CONTAINS);}
"equals"					{return TOKEN(EQUALS);}
{letter}({letter}|{digit})*	{SAVE_TOKEN; return ID;}
{digit}+					{SAVE_TOKEN; return INT;}
 "["("."|({digit}+(","{digit}+)*)|({id}(","{id})*))"]"	{return TOKEN(VALUE);}
"="							{return TOKEN(EQUAL);}
"*"							{return TOKEN(STAR);}
"("							{return TOKEN(LPAREN);}
")"							{return TOKEN(RPAREN);}
"{"							{return TOKEN(LBRACKET);}
"}"							{return TOKEN(RBRACKET);}
.							{printf("Unknown token %s!\n", sumotext); yyterminate();}

%%
namespace PetriEngine{ namespace PQL{
SUMoQuery ParseSUMoQuery(const std::string& queryString){
	sumoQuery = NULL;

	//Load up input buffer in Flex
	YY_BUFFER_STATE buf = sumo_scan_string(queryString.c_str());

	SUMoQuery q;
	q.name = "";
	q.query = NULL;
	q.isInvariant = isInvariant;
	if(sumoparse() == 0){
		q.query = sumoQuery;
		q.name = sumoName;
		q.isInvariant = isInvariant;
	}
	//Delete buffer
	sumo_delete_buffer(buf);
	return q;
}
}}

