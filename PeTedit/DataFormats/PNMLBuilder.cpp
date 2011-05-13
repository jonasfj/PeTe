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
#include "PNMLBuilder.h"

#include <QString>

PNMLBuilder::PNMLBuilder(QIODevice* device)
{
	xml.setDevice(device);
	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(5);
	xml.writeStartDocument("1.0");
	xml.writeStartElement("pnml");
	xml.writeStartElement("net");
}

void PNMLBuilder::addPlace(const std::string &name, int tokens, double x, double y){
	xml.writeStartElement("place");
	xml.writeAttribute("id", name.c_str());
	graphics(x, y);
	this->name(name);
	xml.writeStartElement("initialMarking");
	xml.writeTextElement("value", QString::number(tokens));
	xml.writeEndElement();
	xml.writeEndElement();
}

void PNMLBuilder::addVariable(const std::string& name, int initialValue, int range){
	xml.writeEmptyElement("variable");
	xml.writeAttribute("name", name.c_str());
	xml.writeAttribute("initial-value", QString::number(initialValue));
	xml.writeAttribute("range", QString::number(range));
}


void PNMLBuilder::addTransition(const std::string &name,
								const std::string &conditions,
								const std::string &assignments,
								double x, double y){
	xml.writeStartElement("transition");
	xml.writeAttribute("id", name.c_str());
	graphics(x, y);
	this->name(name);
	xml.writeTextElement("conditions", conditions.c_str());
	xml.writeTextElement("assignments", assignments.c_str());
	xml.writeEndElement();
}

void PNMLBuilder::addInputArc(const std::string &place, const std::string &transition, int weight){
	xml.writeStartElement("arc");
	xml.writeAttribute("source", place.c_str());
	xml.writeAttribute("target", transition.c_str());
	xml.writeStartElement("inscription");
	xml.writeTextElement("value", QString::number(weight));
	xml.writeEndElement();
	xml.writeEndElement();
}

void PNMLBuilder::addOutputArc(const std::string &transition, const std::string &place, int weight){
	xml.writeStartElement("arc");
	xml.writeAttribute("source", transition.c_str());
	xml.writeAttribute("target", place.c_str());
	xml.writeStartElement("inscription");
	xml.writeTextElement("value", QString::number(weight));
	xml.writeEndElement();
	xml.writeEndElement();
}

void PNMLBuilder::makePNMLFile(){
	xml.writeEndElement(); //net

	if(_queries.size() > 0){
		xml.writeStartElement("queries");
		for(int i = 0; i < _queries.size(); i++){
			xml.writeStartElement("query");
			xml.writeAttribute("name", _queries[i].name);
			xml.writeAttribute("strategy", _queries[i].strategy);
			xml.writeAttribute("jit", _queries[i].jit ? "true" : "false");
			xml.writeCharacters(_queries[i].query);
			xml.writeEndElement();
		}
		xml.writeEndElement();
	}
	_queries.clear();

	xml.writeEndElement(); //pnml
	xml.writeEndDocument();
}

void PNMLBuilder::graphics(double x, double y){
	xml.writeStartElement("graphics");
	xml.writeEmptyElement("position");
	xml.writeAttribute("x", QString::number(x));
	xml.writeAttribute("y", QString::number(y));
	xml.writeEndElement();
}

void PNMLBuilder::name(std::string name){
	xml.writeStartElement("name");
	xml.writeTextElement("value", name.c_str());
	xml.writeEndElement();
}

void PNMLBuilder::addQuery(const QueryModel::Query& query){
	_queries.append(query);
}
