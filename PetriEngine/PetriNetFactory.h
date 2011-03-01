#ifndef PETRINETFACTORY_H
#define PETRINETFACTORY_H

#include "AbstractPetriNetFactory.h"
#include <vector>
#include <string>

#include "PetriNet.h"

namespace PetriEngine{


/** Factory for building PetriNets */
class PetriNetFactory : public AbstractPetriNetFactory
{
	struct Arc{
		std::string place;
		std::string transition;
		int weight;
	};
public:
    PetriNetFactory();
	void addPlace(const std::string& name, int tokens, double x, double y);
	void addTransition(const std::string& name, double x, double y);
	void addInputArc(const std::string& place, const std::string& transition, int weight);
	void addOutputArc(const std::string& transition, const std::string& place, int weight);
	/** Make the resulting petri net */
	PetriNet* makePetriNet();
	/** Make the resulting initial marking */
	Mark* makeInitialMarking();
private:
	std::vector<std::string> places;
	std::vector<std::string> transitions;
	std::vector<Arc> inputArcs;
	std::vector<Arc> outputArcs;
	std::vector<int> initialMarking;
};

}

#endif // PETRINETFACTORY_H
