#ifndef CTLPARSER_H
#define CTLPARSER_H

#include "../petrinet.h";
#include "ctlexpr.h"

namespace PetriEngine{ namespace CTL{
class CTLParser {
public:
	CTLParser(PetriNet* network) {
		this->_network = network;
	}
private:
	PetriNet* _network;
};
}}

#endif // CTLPARSER_H
