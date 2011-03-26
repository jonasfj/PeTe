#ifndef PQLPARSER_H
#define PQLPARSER_H

#include <string>
#include "PQLExpressions.h"

namespace PetriEngine { namespace PQL {

Condition* ParseQuery(const std::string& queryString);
AssignmentExpression* ParseAssignment(const std::string& assignmentString);

}}
#endif // PQLPARSER_H
