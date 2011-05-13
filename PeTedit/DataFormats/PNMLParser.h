/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PNMLPARSER_H
#define PNMLPARSER_H

#include <PetriEngine/AbstractPetriNetBuilder.h>
#include "../Misc/AbstractQueryListBuilder.h"

class PetriNetSceneBuilder;

#include <QXmlStreamReader>
#include <QIODevice>
#include <QHash>
#include <QList>

/** Simple PNML Parser for PNDVs */
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
	PNMLParser() { builder = NULL; qBuilder = NULL; }
	/** Parse input and build result with builder */
	void parse(QIODevice* input, PetriEngine::AbstractPetriNetBuilder* builder, AbstractQueryListBuilder* qbuilder = NULL);
private:
	void pnml();
	void queries();
	void net();
	void variable();
	void place();
	void transition();
	void arc();
	void position(qreal& x, qreal& y);
	void value(QString& value);
	/** Builder for creating new petri net */
	PetriEngine::AbstractPetriNetBuilder* builder;
	/** Query builder */
	AbstractQueryListBuilder* qBuilder;
	QXmlStreamReader xml;
	QList<ArcEntry> arcs;
	/** Maps Ids to names */
	QHash<QString,NodeName> idmap;
};

#endif // PNMLPARSER_H
