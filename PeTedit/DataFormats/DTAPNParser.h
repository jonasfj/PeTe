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
#ifndef DTAPNPARSER_H
#define DTAPNPARSER_H

#include <PetriEngine/DTAPN/AbstractDTAPNBuilder.h>

#include <QXmlStreamReader>
#include <QIODevice>
#include <QHash>
#include <QList>

/** PNML Parser for DTAPNs */
class DTAPNParser
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
		ArcEntry(QString src, QString dst, int startInterval, int endInterval){
			this->src = src;
			this->dst = dst;
			this->startInterval = startInterval;
			this->endInterval = endInterval;
		}
		QString src;
		QString dst;
		/** start interval for the arc (discard if out-arc) */
		int startInterval;
		/** end interval for the arc (discard if out-arc) */
		int endInterval;
	};
public:
	struct Query{
		QString name;
		QString query;
	};

	DTAPNParser() { builder = NULL; }
	/** Parse input and build result with builder */
	void parse(QIODevice* input, PetriEngine::DTAPN::AbstractDTAPNBuilder* builder);
	QList<Query> getQueries() { return _queries; }
private:
	void pnml();
	void net();
	void place();
	void transition();
	void arc();
	void query();
	void queries();
	void position(qreal& x, qreal& y);
	void value(QString& value);
	/** Builder for creating new petri net */
	PetriEngine::DTAPN::AbstractDTAPNBuilder* builder;
	QXmlStreamReader xml;
	QList<ArcEntry> arcs;
	/** Maps Ids to names */
	QHash<QString, NodeName> idmap;
	QList<Query> _queries;
};


#endif // DTAPNPARSER_H
