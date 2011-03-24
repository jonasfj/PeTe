#ifndef PQLPARSER_H
#define PQLPARSER_H

#include <string>
#include "PQLExpressions.h"

PetriEngine::PQL::Condition* pqlParseQuery(const std::string& queryString);
PetriEngine::PQL::AssignmentExpression* pqlParseAssignment(const std::string& assignmentString);

#endif // PQLPARSER_H
