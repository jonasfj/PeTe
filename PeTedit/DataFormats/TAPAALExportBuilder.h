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
#ifndef TAPAALEXPORTBUILDER_H
#define TAPAALEXPORTBUILDER_H

#include <PetriEngine/AbstractPetriNetBuilder.h>

#include "../Misc/QueryModel.h"

#include <QIODevice>
#include <QStringBuilder>
#include <QXmlStreamWriter>

/** Builder for exporting to TAPAAL */
class TAPAALExportBuilder : public PetriEngine::AbstractPetriNetBuilder
{
public:
	TAPAALExportBuilder(QIODevice* device);
	void addPlace(const std::string &name, int tokens, double x, double y);
	void addVariable(const std::string& name, int initialValue, int range);
	void addTransition(const std::string &name,
					   const std::string &conditions,
					   const std::string &assignments,
					   double x, double y);
	void addInputArc(const std::string &place, const std::string &transition, int weight);
	void addOutputArc(const std::string &transition, const std::string &place, int weight);
	void makeXMLFile();

	/** Save a query with the output, this method is not a part of the builder interface */
	void addQuery(const QueryModel::Query& query);
private:
	QString _comments;
	QList<QueryModel::Query> _queries;
	QXmlStreamWriter xml;
	int _nextArcId;
	double _maxX;
};

#endif // TAPAALEXPORTBUILDER_H
