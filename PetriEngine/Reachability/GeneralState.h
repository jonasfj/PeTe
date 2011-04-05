#ifndef GENERALSTATE_H
#define GENERALSTATE_H

#include "../PetriNet.h"

namespace PetriEngine { namespace Reachability {

/** GeneralState class for reachability searches.
  * Used in most reachability search cases */
class GenericState {
public:
	MarkVal* marking(){
		return _marking;
	}
	VarVal* valuation(){
		return _valuation;
	}

	GenericState* parent(){
		return _parent;
	}

	/** Check if this state is a loop */
	bool isLoop(const PetriNet& net){

		GenericState* parent = this->_parent;

		if(!parent)
			return false;

		while(parent){
			bool isLoop = true;
			//Check assignment
			for(unsigned int i = 0; i < net.numberOfVariables(); i++){
				isLoop &= _valuation[i] == parent->_valuation[i];
				if(!isLoop)
					break;
			}

			//Check marking
			for(unsigned int i = 0; i < net.numberOfPlaces(); i++){
				isLoop &= _marking[i] == parent->_marking[i];
				if(!isLoop)
					break;
			}

			//While loop maintenance
			if(isLoop)
				return true;
			parent = parent->_parent;

		}

		return false;
	}

	/** Create a new state */
	static inline GenericState* createState(int nPlaces, int nVars, GenericState* parent = NULL) {
		char *d = (char*)calloc(1, sizeof(GenericState) + sizeof(MarkVal)*nPlaces + sizeof(VarVal)*nVars);
		GenericState* s = (GenericState*)d;
		s->_marking = (MarkVal*)(d + sizeof(GenericState));
		s->_valuation = (VarVal*)(d+ sizeof(GenericState) + sizeof(MarkVal) * nPlaces);
		if(parent)
			s->_parent = parent;
		return (GenericState*)d;
	}
private:
	GenericState* _parent;
	MarkVal* _marking;
	VarVal* _valuation;
};

}}

#endif //GENERALSTATE_H
