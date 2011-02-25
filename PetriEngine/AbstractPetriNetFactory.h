#ifndef ABSTRACTPETRINETFACTORY_H
#define ABSTRACTPETRINETFACTORY_H

#include <string>

namespace PetriEngine{

/** Abstract factory for petri nets */
class AbstractPetriNetFactory{
public:
	/** Add a new place with a unique name */
	virtual void addPlace(const std::string& name, double x, double y) = 0;
	/** Add a new transition with a unique name */
	virtual void addTransition(const std::string& name, double x, double y) = 0;
	/** Add input arc with given weight */
	virtual void addInputArc(const std::string& place, const std::string& transition, int weight = 1) = 0;
	/** Add output arc with given weight */
	virtual void addOutputArc(const std::string& transition, const std::string& place, int weight = 1) = 0;
};

}

#endif // ABSTRACTPETRINETFACTORY_H
