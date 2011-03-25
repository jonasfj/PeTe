#ifndef PQLPARSER_H
#define PQLPARSER_H

#include <string>
#include "PQLExpressions.h"

namespace PetriEngine { namespace PQL {

class PQLParser {
public:
	PQLParser();
	Condition* parseQuery(const std::string& query);
};


Condition* pqlParseQuery(const std::string& queryString);
AssignmentExpression* pqlParseAssignment(const std::string& assignmentString);
}}
#endif // PQLPARSER_H
