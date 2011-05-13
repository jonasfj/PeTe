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
#include "PNMLParser.h"

#include <string>

using namespace PetriEngine;
using namespace XMLSP;
using namespace std;

void PNMLParser::parse(const std::string& xml,
					   AbstractPetriNetBuilder* builder){
	//Clear any left overs
	id2name.clear();
	arcs.clear();

	//Set the builder
	this->builder = builder;

	//Parser the xml
	DOMElement* root = DOMElement::loadXML(xml);
	parseElement(root);

	//Add all the arcs
	for(ArcIter it = arcs.begin(); it != arcs.end(); it++){
		//Check that source id exists
		if(id2name.find(it->source) == id2name.end()){
			fprintf(stderr,
					"XML Parsing error: Arc source with id=\"%s\" wasn't found!\n",
					it->source.c_str());
			continue;
		}
		//Check that target id exists
		if(id2name.find(it->target) == id2name.end()){
			fprintf(stderr,
					"XML Parsing error: Arc target with id=\"%s\" wasn't found!\n",
					it->target.c_str());
			continue;
		}
		//Find source and target
		NodeName source = id2name[it->source];
		NodeName target = id2name[it->target];

		if(source.isPlace && !target.isPlace){
			builder->addInputArc(source.name, target.name, it->weight);
		}else if(!source.isPlace && target.isPlace){
			builder->addOutputArc(source.name, target.name, it->weight);
		}else{
			fprintf(stderr,
					"XML Parsing error: Arc from \"%s\" to \"%s\" is neight input nor output!\n",
					source.name.c_str(),
					target.name.c_str());
		}
	}

	//Release DOM tree
	delete root;

	//Unset the builder
	this->builder = NULL;

	//Cleanup
	id2name.clear();
	arcs.clear();
}

void PNMLParser::makePetriNet(){
	builder = NULL;
}

void PNMLParser::parseElement(DOMElement* element){
	DOMElements elements = element->getChilds();
	DOMElements::iterator it;
	for(it = elements.begin(); it != elements.end(); it++){
		if((*it)->getElementName() == "place"){
			parsePlace(*it);
		}else if((*it)->getElementName() == "transition"){
			parseTransition(*it);
		}else if((*it)->getElementName() == "arc"){
			parseArc(*it);
		}else if((*it)->getElementName() == "variable"){
			parseVariable(*it);
		} else if((*it)->getElementName() == "queries"){
			parseQueries(*it);
		}else
			parseElement(*it);
	}
}

void PNMLParser::parseQueries(DOMElement* element){
	string name, query;

	DOMElements elements = element->getChilds();
	DOMElements::iterator it;
	for(it = elements.begin(); it != elements.end(); it++){
		name = element->getAttribute("name");
		parseValue(element,query);
		Query q;
		q.name = name;
		q.text = query;
		this->queries.push_back(q);
	}
}

void PNMLParser::parsePlace(DOMElement* element){
	double x = 0, y = 0;
	string name, id = element->getAttribute("id");
	int initialMarking = 0;

	DOMElements elements = element->getChilds();
	DOMElements::iterator it;
	for(it = elements.begin(); it != elements.end(); it++){
		if((*it)->getElementName() == "name"){
			parseValue(*it, name);
		}else if((*it)->getElementName() == "graphics"){
			parsePosition(*it, x, y);
		}else if((*it)->getElementName() == "initialMarking"){
			string text;
			parseValue(*it, text);
			initialMarking = atoi(text.c_str());
		}
	}
	//Create place
	builder->addPlace(name, initialMarking, x, y);
	//Map id to name
	NodeName nn;
	nn.name = name;
	nn.isPlace = true;
	id2name[id] = nn;
}

void PNMLParser::parseArc(DOMElement* element){
	string source = element->getAttribute("source"),
		   target = element->getAttribute("target");
	int weight = 1;

	DOMElements elements = element->getChilds();
	DOMElements::iterator it;
	for(it = elements.begin(); it != elements.end(); it++){
		if((*it)->getElementName() == "inscription"){
			string text;
			parseValue(*it, text);
			weight = atoi(text.c_str());
		}
	}

	Arc arc;
	arc.source = source;
	arc.target = target;
	arc.weight = weight;
	arcs.push_back(arc);
}

void PNMLParser::parseTransition(DOMElement* element){
	double x = 0, y = 0;
	string name, id = element->getAttribute("id");
	string cond, assign;

	DOMElements elements = element->getChilds();
	DOMElements::iterator it;
	for(it = elements.begin(); it != elements.end(); it++){
		if((*it)->getElementName() == "name"){
			parseValue(*it, name);
		}else if((*it)->getElementName() == "graphics"){
			parsePosition(*it, x, y);
		}else if((*it)->getElementName() == "conditions"){
			cond = (*it)->getCData();
		}else if((*it)->getElementName() == "assignments"){
			assign = (*it)->getCData();
		}
	}
	//Create place
	builder->addTransition(name, cond, assign, x, y);
	//Map id to name
	NodeName nn;
	nn.name = name;
	nn.isPlace = false;
	id2name[id] = nn;
}

void PNMLParser::parseVariable(DOMElement* element){
	string name = element->getAttribute("name");
	int initialValue = atoi(element->getAttribute("initial-value").c_str());
	int range = atoi(element->getAttribute("range").c_str());

	//Add the variable
	builder->addVariable(name, initialValue, range);
}


void PNMLParser::parseValue(DOMElement* element, string& text){
	DOMElements elements = element->getChilds();
	DOMElements::iterator it;
	for(it = elements.begin(); it != elements.end(); it++){
		if((*it)->getElementName() == "value" || (*it)->getElementName() == "text")
			text = (*it)->getCData();
		else
			parseValue(*it, text);
	}
}

void PNMLParser::parsePosition(DOMElement* element, double& x, double& y){
	DOMElements elements = element->getChilds();
	DOMElements::iterator it;
	for(it = elements.begin(); it != elements.end(); it++){
		if((*it)->getElementName() == "position"){
			x = atof((*it)->getAttribute("x").c_str());
			y = atof((*it)->getAttribute("y").c_str());
		}else
			parsePosition(*it, x, y);
	}
}
