#ifndef PNMLBUILDER_H
#define PNMLBUILDER_H

#include <PetriEngine/AbstractPetriNetBuilder.h>

#include "../Misc/QueryModel.h"

#include <QIODevice>
#include <QXmlStreamWriter>

/** Builder for PNML export */
class PNMLBuilder : public PetriEngine::AbstractPetriNetBuilder
{
public:
	PNMLBuilder(QIODevice* device);
	void addPlace(const std::string &name, int tokens, double x, double y);
	void addVariable(const std::string& name, int initialValue, int range);
	void addTransition(const std::string &name,
					   const std::string &conditions,
					   const std::string &assignments,
					   double x, double y);
	void addInputArc(const std::string &place, const std::string &transition, int weight);
	void addOutputArc(const std::string &transition, const std::string &place, int weight);
	void makePNMLFile();

	/** Save a query with the output, this method is specific to this class */
	void addQuery(const QueryModel::Query& query);
private:
	QList<QueryModel::Query> _queries;
	QXmlStreamWriter xml;
	void graphics(double x, double y);
	void name(std::string name);
};

#endif // PNMLBUILDER_H
