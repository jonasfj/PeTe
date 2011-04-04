#ifndef GENERALSTATE_H
#define GENERALSTATE_H

#include "../PetriNet.h"

namespace PetriEngine { namespace Reachability {

/** GeneralState class for reachability searches.
  * Used in most reachability search cases */
class GeneralState {
public:
	MarkVal* marking(){
		return _marking;
	}
	VarVal* valuation(){
		return _valuation;
	}

	/** Check if this state is a loop */
	bool isLoop(const PetriNet& net){

		GeneralState* parent = this->_parent;

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
	static inline GeneralState* createState(int nPlaces, int nVars, GeneralState* parent = NULL) {
		char *d = (char*)calloc(1, sizeof(GeneralState) + sizeof(MarkVal)*nPlaces + sizeof(VarVal)*nVars);
		GeneralState* s = (GeneralState*)d;
		s->_marking = (MarkVal*)(d + sizeof(GeneralState));
		s->_valuation = (VarVal*)(d+ sizeof(GeneralState) + sizeof(MarkVal) * nPlaces);
		if(parent)
			s->_parent = parent;
		return (GeneralState*)d;
	}
private:
	GeneralState* _parent;
	MarkVal* _marking;
	VarVal* _valuation;
};

}}

#endif //GENERALSTATE_H
