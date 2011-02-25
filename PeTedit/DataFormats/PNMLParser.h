#ifndef PNMLPARSER_H
#define PNMLPARSER_H

#include "AbstractPetriNetFactory.h"

#include <QXmlStreamReader>
#include <QIODevice>
#include <QHash>

/** Simple PNML Parser for basic petri nets */
class PNMLParser
{
	/** Types if nodes that is assigned an id */
	enum NodeType{
		Place,
		Transition
	};
	/** Tuple of name and place/transition designation */
	struct NodeName{
		NodeType type;
		QString name;
	};
public:
	PNMLParser() { factory = NULL; }
	/** Parse input and build result with factory */
	void parse(QIODevice* input, PetriEngine::AbstractPetriNetFactory* factory);
private:
	void pnml();
	void net();
	void place();
	void transition();
	void arc();
	void position(qreal& x, qreal& y);
	void value(QString& value);
	/** Factory for creating new petri net */
	PetriEngine::AbstractPetriNetFactory* factory;
	QXmlStreamReader xml;
	/** Maps Ids to names */
	QHash<QString,NodeName> idmap;
};

#endif // PNMLPARSER_H
