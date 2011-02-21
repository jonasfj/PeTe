#include "petrinet.h"

PetriNet::PetriNet(int places, int transitions){
	_nPlaces = places;
	_nTransitions = transitions;
	_placeNames = new string[places];
	_transitionNames = new string[transitions];
	_transitions = new Mark[places*transitions];
}

bool PetriNet::fire(int transition, const Mark *marking, Mark* result) const{
	Mark* t = _transitions + transition * _nPlaces;
	for(int i = 0; i < _nPlaces; i++){
		result[i] = marking[i] + t[i];
		if(result[i] < 0)
			return false;
	}
	return true;
}
