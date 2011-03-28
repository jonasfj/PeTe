#ifndef PNMLFACTORY_H
#define PNMLFACTORY_H

#include "AbstractPetriNetFactory.h"

#include <QIODevice>
#include <QXmlStreamWriter>

/** Factory for PNML export */
class PNMLFactory : public PetriEngine::AbstractPetriNetFactory
{
public:
	PNMLFactory(QIODevice* device);
	void addPlace(const std::string &name, int tokens, double x, double y);
	void addVariable(const std::string& name, int initialValue, int range);
	void addTransition(const std::string &name,
					   const std::string &conditions,
					   const std::string &assignments,
					   double x, double y);
	void addInputArc(const std::string &place, const std::string &transition, int weight);
	void addOutputArc(const std::string &transition, const std::string &place, int weight);
	void makePNMLFile();
private:
	QXmlStreamWriter xml;
	void graphics(double x, double y);
	void name(std::string name);
};

#endif // PNMLFACTORY_H
