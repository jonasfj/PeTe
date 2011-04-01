#ifndef PNMLPARSER_H
#define PNMLPARSER_H

#include "AbstractPetriNetBuilder.h"

#include <QXmlStreamReader>
#include <QIODevice>
#include <QHash>
#include <QList>

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
		NodeName(){}
		NodeName(NodeType type, QString name){
			this->type = type;
			this->name = name;
		}
		NodeType type;
		QString name;
	};
	/** Arc entry, because we can't create arcs before all nodes are created */
	struct ArcEntry{
		ArcEntry(){}
		ArcEntry(QString src, QString dst, int weight){
			this->src = src;
			this->dst = dst;
			this->weight = weight;
		}
		QString src;
		QString dst;
		int weight;
	};
public:
	PNMLParser() { builder = NULL; }
	/** Parse input and build result with builder */
	void parse(QIODevice* input, PetriEngine::AbstractPetriNetBuilder* builder);
private:
	void pnml();
	void net();
	void variable();
	void place();
	void transition();
	void arc();
	void position(qreal& x, qreal& y);
	void value(QString& value);
	/** Builder for creating new petri net */
	PetriEngine::AbstractPetriNetBuilder* builder;
	QXmlStreamReader xml;
	QList<ArcEntry> arcs;
	/** Maps Ids to names */
	QHash<QString,NodeName> idmap;
};

#endif // PNMLPARSER_H
