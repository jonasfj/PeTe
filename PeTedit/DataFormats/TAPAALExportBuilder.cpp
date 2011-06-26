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
#include "TAPAALExportBuilder.h"

#include <PetriEngine/PQL/PQLParser.h>
#include <PetriEngine/PQL/PQL.h>
#include <PetriEngine/PQL/Contexts.h>

#include <QString>

TAPAALExportBuilder::TAPAALExportBuilder(QIODevice* device)
{
	xml.setDevice(device);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(5);
	xml.writeStartDocument("1.0");
	xml.writeStartElement("pnml");
	xml.writeAttribute("xmlns", "http://www.informatik.hu-berlin.de/top/pnml/ptNetb");
	xml.writeStartElement("net");
	xml.writeAttribute("active", "true");
	xml.writeAttribute("id", "ExportedNet");
	xml.writeAttribute("type", "P/T net");
	_nextArcId = 0;
	_maxX = 0;
}

void TAPAALExportBuilder::addPlace(const std::string &name, int tokens, double x, double y){
	xml.writeEmptyElement("place");
	xml.writeAttribute("id", name.c_str());
	xml.writeAttribute("initialMarking", QString::number(tokens));
	xml.writeAttribute("invariant", "< inf");
	xml.writeAttribute("markingOffsetX", "0.0");
	xml.writeAttribute("markingOffsetY", "0.0");
	xml.writeAttribute("name", name.c_str());
	xml.writeAttribute("nameOffsetX", "-5.0");
	xml.writeAttribute("nameOffsetY", "35.0");
	xml.writeAttribute("positionX", QString::number(x));
	xml.writeAttribute("positionY", QString::number(y));
	_maxX = _maxX > x ? _maxX : x;
}

void TAPAALExportBuilder::addVariable(const std::string& name, int initialValue, int range){
	_comments += "Variable \"" + QString(name.c_str()) + "\" = " + QString::number(initialValue) +" with upperbound " + QString::number(range) + " was ignored\n";
}


void TAPAALExportBuilder::addTransition(const std::string &name,
								const std::string &conditions,
								const std::string &assignments,
								double x, double y){
	xml.writeEmptyElement("transition");
	xml.writeAttribute("angle", "0");
	xml.writeAttribute("id", name.c_str());
	xml.writeAttribute("infiniteServer", "false");
	xml.writeAttribute("name", name.c_str());
	xml.writeAttribute("nameOffsetX", "-5.0");
	xml.writeAttribute("nameOffsetY", "35.0");
	xml.writeAttribute("positionX", QString::number(x));
	xml.writeAttribute("positionY", QString::number(y));
	xml.writeAttribute("priority", "1");;
	xml.writeAttribute("timed", "true");
	if(!conditions.empty())
		_comments += "Pre-conditions \"" + QString(conditions.c_str()).replace("\n", "") + "\" on + \"" + name.c_str() + "\" was ignored.\n";
	if(!assignments.empty())
		_comments += "Post-assignments \"" + QString(assignments.c_str()).replace("\n", "") + "\" on + \"" + name.c_str() + "\" was ignored.\n";
	_maxX = _maxX > x ? _maxX : x;
}

void TAPAALExportBuilder::addInputArc(const std::string &place, const std::string &transition, int weight){
	if(weight == 0) return;
	xml.writeStartElement("arc");
	xml.writeAttribute("source", place.c_str());
	xml.writeAttribute("target", transition.c_str());
	xml.writeAttribute("inscription", "[0,inf)");
	xml.writeAttribute("type", "timed");
	xml.writeAttribute("id", "arc" + QString::number(_nextArcId++));
	xml.writeEndElement();
	if(weight > 1)
		_comments += "Weight " + QString::number(weight) + " on arc from \"" + place.c_str() + "\" to \"" + transition.c_str() + "\" was reduced to 1.\n";
}

void TAPAALExportBuilder::addOutputArc(const std::string &transition, const std::string &place, int weight){
	if(weight == 0) return;
	xml.writeStartElement("arc");
	xml.writeAttribute("source", transition.c_str());
	xml.writeAttribute("target", place.c_str());
	xml.writeAttribute("inscription", "1");
	xml.writeAttribute("type", "normal");
	xml.writeAttribute("id", "arc" + QString::number(_nextArcId++));
	xml.writeEndElement();
	if(weight > 1)
		_comments += "Weight " + QString::number(weight) + " on arc from \"" + transition.c_str() + "\" to \"" + place.c_str() + "\" was reduced to 1.\n";
}

void TAPAALExportBuilder::addQuery(const QueryModel::Query& query){
	PetriEngine::PQL::Condition* q = PetriEngine::PQL::ParseQuery(query.query.toStdString());
	if(!q){
		QString qs = query.query;
		_comments += "Query \"" + query.name + "\" = \"" + qs.replace("\n","") + "\" didn't parse.\n";
		return;
	}else{
		PetriEngine::PQL::TAPAALConditionExportContext context;
		context.failed = false;
		context.netName = "ExportedNet";
		QueryModel::Query cpy = query;
		cpy.query = "EF ( " + QString(q->toTAPAALQuery(context).c_str()) + " )";
		if(!context.failed)
			_queries.append(cpy);
		else{
			QString qs = query.query;
			_comments += "Query \"" + query.name + "\" = \"" + qs.replace("\n","")+ "\" couldn't be translated.\n";
		}
	}
}


void TAPAALExportBuilder::makeXMLFile(){
	if(!_comments.isEmpty()){
		xml.writeStartElement("labels");
		xml.writeAttribute("border", "true");
		xml.writeAttribute("positionX", QString::number(_maxX + 50));
		xml.writeAttribute("positionY", "50");
		xml.writeAttribute("height", "700");
		xml.writeAttribute("width", "500");
		xml.writeCharacters(_comments);
		xml.writeEndElement();
	}
	xml.writeEndElement(); //net

	if(_queries.size() > 0){
		for(int i = 0; i < _queries.size(); i++){
			xml.writeEmptyElement("query");
			xml.writeAttribute("active", "true");
			xml.writeAttribute("capacity", "0");
			xml.writeAttribute("discreteInclusion", "false");
			xml.writeAttribute("extrapolationOption", "null");
			xml.writeAttribute("hashTableSize", "null");
			xml.writeAttribute("inclusionPlaces", "*NONE*");
			xml.writeAttribute("name", _queries[i].name);
			xml.writeAttribute("query", _queries[i].query);
			xml.writeAttribute("reductionOption", "VerifyTAPN");
			xml.writeAttribute("searchOption", "HEURISTIC");
			xml.writeAttribute("symmetry", "true");
			xml.writeAttribute("traceOption", "NONE");
		}
	}
	_queries.clear();

	xml.writeEndElement(); //pnml
	xml.writeEndDocument();
}
