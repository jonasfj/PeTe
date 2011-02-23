#ifndef ABSTRACTPETRINETFACTORY_H
#define ABSTRACTPETRINETFACTORY_H

#include <string>

using namespace std;

/** Abstract factory for petri nets */
class AbstractPetriNetFactory{
public:
	/** Add a new place with a unique name */
	virtual void addPlace(const string& name) = 0;
	/** Add a new transition with a unique name */
	virtual void addTransition(const string& name) = 0;
	/** Add input arc with given weight */
	virtual void addInputArc(const string& place, const string& transition, int weight = 1) = 0;
	/** Add output arc with given weight */
	virtual void addOutputArc(const string& transition, const string& place, int weight = 1) = 0;
};

#endif // ABSTRACTPETRINETFACTORY_H
