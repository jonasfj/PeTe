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
#include "xmlsp/xmlsp_dom.h"

#include <map>
#include <string>
#include <vector>

class PNMLParser
{

	struct Arc{
		std::string source,
					target;
		int weight;
	};
	typedef std::vector<Arc> ArcList;
	typedef ArcList::iterator ArcIter;
	struct NodeName{
		std::string name;
		bool isPlace;
	};
	typedef std::map<std::string, NodeName> NodeNameMap;
public:

	struct Query{
		std::string name,text;
	};

	PNMLParser(){
		builder = NULL;
	}
	void parse(const std::string& xml,
			   PetriEngine::AbstractPetriNetBuilder* builder);
	void makePetriNet();
	std::vector<Query> getQueries(){
		return queries;
	}

private:
	void parseElement(XMLSP::DOMElement* element);
	void parsePlace(XMLSP::DOMElement* element);
	void parseArc(XMLSP::DOMElement* element);
	void parseTransition(XMLSP::DOMElement* element);
	void parseVariable(XMLSP::DOMElement* element);
	void parseValue(XMLSP::DOMElement* element, std::string& text);
	void parsePosition(XMLSP::DOMElement* element, double& x, double& y);
	void parseQueries(XMLSP::DOMElement* element);
	PetriEngine::AbstractPetriNetBuilder* builder;
	NodeNameMap id2name;
	ArcList arcs;
	std::vector<Query> queries;
};

#endif // PNMLPARSER_H
