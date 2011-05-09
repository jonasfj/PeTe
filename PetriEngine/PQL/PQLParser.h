#ifndef PQLPARSER_H
#define PQLPARSER_H

#include <string>

namespace PetriEngine { namespace PQL {

class Condition;
class AssignmentExpression;

/** Return value for SUMo import */
struct SUMoQuery{
	Condition* query;
	std::string name;
	bool isInvariant;
};

Condition* ParseQuery(const std::string& queryString);
SUMoQuery ParseSUMoQuery(const std::string& queryString);
AssignmentExpression* ParseAssignment(const std::string& assignmentString);

}}
#endif // PQLPARSER_H
