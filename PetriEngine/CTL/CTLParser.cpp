#include <sstream>
#include "CTLParser.h"

using namespace std;

PetriEngine::CTL::CTLExpr* ctlParseString(const string& queryString);

namespace PetriEngine { namespace CTL {

CTLExpr* CTLParser::parse(const string& queryString) {
	//Load up input buffer in Flex
	return ctlParseString(queryString);
}

}}
