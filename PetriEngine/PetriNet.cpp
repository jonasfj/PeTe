#include "PetriNet.h"
#include <stdio.h>
using namespace std;

namespace PetriEngine{

PetriEngine::PetriNet::PetriNet(int places, int transitions){
	_nPlaces = places;
	_nTransitions = transitions;
	_placeNames = new string[places];
	_transitionNames = new string[transitions];
	_transitions = NEW_MARKING(places*transitions);
	//Set transition matrix to zero
	for(int i = 0; i < places*transitions; i++)
		_transitions[i] = 0;
}

bool PetriEngine::PetriNet::fire(int transition, const Marking marking, Marking result) const{
	Marking t = _transitions + transition * _nPlaces;
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
Marking PetriNet::makeEmptyMarking(){
	ALLOCATE_MARKING(marking,this->_nPlaces);
	for(int i = 0; i < this->_nPlaces; i++)
		SET_TOKENS(marking,i,0);
	return marking;
}

/** Returns the Place offset */
int PetriNet::lookupPlace(const string &name) const{
	for(int i = 0; i < _nPlaces; i++){
		if(_placeNames[i] == name)
			return i;
	}
	return -1;
}

int PetriNet::lookupVariable(const string &name) const{
	return -1;
}

} // PetriEngine
