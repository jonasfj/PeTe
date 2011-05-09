#include "SmartState.h"

#include <string.h>

namespace PetriEngine{
namespace Structures{

int SmartState::getState(const PetriNet& net, MarkVal* marking, VarVal* valuation) const{
	if(stored()){
		memcpy(marking, _marking, sizeof(MarkVal) * net.numberOfPlaces());
		memcpy(valuation, _valuation, sizeof(VarVal) * net.numberOfVariables());
		return 0;
	}
	int depth = parent()->getState(net, marking, valuation);
	net.fireWithoutCheck(transition(), marking, valuation, marking, valuation, multiplicity());
	return depth + 1;
}

} // Structures
} // PetriEngine
