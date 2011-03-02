#ifndef CTLPARSER_H
#define CTLPARSER_H

#include <string>
#include "../PetriNet.h"
#include "CTLExprs.h"

namespace PetriEngine{ namespace CTL{
class CTLParser {
public:
	CTLParser(PetriNet* network) {
		this->_network = network;
	}

	CTLExpr* parse(const std::string& queryString);
private:
	PetriNet* _network;
};
}}

#endif // CTLPARSER_H
