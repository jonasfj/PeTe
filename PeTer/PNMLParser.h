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
