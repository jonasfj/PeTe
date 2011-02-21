#ifndef PETRINETFACTORY_H
#define PETRINETFACTORY_H

#include "abstractpetrinetfactory.h"
#include "petrinet.h"
#include <vector>
#include <string>

using namespace std;

/** Factory for building PetriNets */
class PetriNetFactory : public AbstractPetriNetFactory
{
	struct Arc{
		string place;
		string transition;
		int weight;
	};
public:
    PetriNetFactory();
	void addPlace(const string& name);
	void addTransition(const string& name);
	void addInputArc(const string& place, const string& transition, int weight);
	void addOutputArc(const string& transition, const string& place, int weight);
	PetriNet makePetriNet();
private:
	vector<string> places;
	vector<string> transitions;
	vector<Arc> inputArcs;
	vector<Arc> outputArcs;
};

#endif // PETRINETFACTORY_H
