#ifndef PETRINETBUILDER_H
#define PETRINETBUILDER_H

#include "AbstractPetriNetBuilder.h"
#include <vector>
#include <string>

#include "PetriNet.h"

namespace PetriEngine{


/** Builder for building engine representations of PetriNets */
class PetriNetBuilder : public AbstractPetriNetBuilder
{
	struct Arc{
		std::string place;
		std::string transition;
		int weight;
	};
public:
	PetriNetBuilder();
	void addVariable(const std::string& name, int initialValue, int range);
	void addPlace(const std::string& name, int tokens, double x, double y);
	void addTransition(const std::string& name,
					   const std::string& condition,
					   const std::string& assignment,
					   double x,
					   double y);
	void addInputArc(const std::string& place,
					 const std::string& transition,
					 int weight);
	void addOutputArc(const std::string& transition, const std::string& place, int weight);
	/** Make the resulting petri net, you take ownership */
	PetriNet* makePetriNet();
	/** Make the resulting initial marking, you take ownership */
	MarkVal* makeInitialMarking();
	/** Make the resulting initial assignment, you take ownership */
	VarVal* makeInitialAssignment();
private:
	std::vector<std::string> places;
	std::vector<std::string> transitions;
	std::vector<std::string> conditions;
	std::vector<std::string> assignments;
	std::vector<std::string> variables;
	std::vector<int> initialVariableValues;
	std::vector<int> ranges;
	std::vector<Arc> inputArcs;
	std::vector<Arc> outputArcs;
	std::vector<int> initialMarking;
};

}

#endif // PETRINETBUILDER_H
