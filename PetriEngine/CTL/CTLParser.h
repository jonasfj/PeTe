#ifndef CTLPARSER_H
#define CTLPARSER_H

#include "../PetriNet.h"
#include "CTLExprs.h"

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
