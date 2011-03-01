#include "PetriNet.h"
#include <stdio.h>
using namespace std;

namespace PetriEngine{

PetriEngine::PetriNet::PetriNet(int places, int transitions){
	_nPlaces = places;
	_nTransitions = transitions;
	_placeNames = new string[places];
	_transitionNames = new string[transitions];
	_transitions = new Mark[places*transitions];
	//Set transition matrix to zero
	for(int i = 0; i < places*transitions; i++)
		_transitions[i] = 0;
}

bool PetriEngine::PetriNet::fire(int transition, const Mark *marking, Mark* result) const{
	Mark* t = _transitions + transition * _nPlaces;
	for(int i = 0; i < _nPlaces; i++){
		result[i] = marking[i] + t[i];
		if(result[i] < 0)
			return false;
	}
	return true;
}

int PetriEngine::PetriNet::nPlaces(){
	return _nPlaces;
}

int PetriEngine::PetriNet::nTransitions(){
	return _nTransitions;
}

/** Inits an empty marking */
Mark* PetriNet::makeEmptyMarking(){
	Mark* marking = new Mark[this->_nPlaces];
	for(int i = 0; i < this->_nPlaces; i++)
		marking[i]=0;
	return marking;
}

/** Returns the Place offset */
int PetriNet::lookupName(const string &name){
	for(int i = 0; i < _nPlaces; i++){
		if(_placeNames[i] == name)
			return i;
	}
	return -1;
}

} // PetriEngine
