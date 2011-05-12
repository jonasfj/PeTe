#ifndef GENERALSTATE_H
#define GENERALSTATE_H

#include "../PetriNet.h"

#include <stdlib.h>
#include <string.h>

namespace PetriEngine { namespace Structures {

template<size_t blocksize> class StateAllocator;
template<size_t blocksize> class BoundedStateAllocator;

/** GeneralState class for reachability searches.
  * Used in most reachability search cases */
class State {
public:
	MarkVal* marking(){return _marking;}
	const MarkVal* marking() const{return _marking;}

	VarVal* valuation(){return _valuation; }
	const VarVal* valuation() const{return _valuation; }

	void setMarking(MarkVal* m){ _marking = m;}
	void setValuation(VarVal* v){ _valuation = v;}

	/** Getter for the parent */
	State* parent(){ return _parent; }
	/** Setter for the parent*/
	void setParent(State* parent){ _parent = parent; }

	/** Getter for the transition the parent took to get here */
	unsigned int transition(){ return _parentTransition; }
	/** Setter for the transition the parent took to get here */
	void setTransition(unsigned int t){ _parentTransition = t; }

	/** Gets the length of the trace to this state */
	int pathLength(){
		if(_parent)
			return 1 + _parent->pathLength();
		else
			return 0;
	}

	/** State specialisation of std::hash */
	class hash : public std::unary_function<State*, size_t>{
	public:
		size_t operator()(const State* state) const{
			//TODO: Rotate bits during hashing
			size_t hash = 0;
			for(unsigned int i = 0; i < nPlaces; i++)
				hash ^=	(state->_marking[i] << (i*4 % (sizeof(MarkVal)*8))) | (state->_marking[i] >> (32 - (i*4 % (sizeof(MarkVal)*8))));
			for(unsigned int i = 0; i < nVariables; i++)
				hash ^= (state->_valuation[i] << (i*4 % (sizeof(VarVal)*8))) | (state->_valuation[i] >> (32 - (i*4 % (sizeof(VarVal)*8))));
			return hash;
		}
		hash(unsigned int places, unsigned int variables)
			: nPlaces(places), nVariables(variables){}
	private:
		unsigned int nPlaces;
		unsigned int nVariables;
	};

	/** State specialisation of std::equal_to */
	class equal_to : public std::binary_function<State*, State*, bool>{
	public:
		bool operator()(const State* state1, const State* state2) const{
			bool equal = true;
			//Check assignment
			for(unsigned int i = 0; i < nVariables; i++){
				equal &= state1->_valuation[i] == state2->_valuation[i];
				if(!equal)
					return false;
			}

			//Check marking
			for(unsigned int i = 0; i < nPlaces; i++){
				equal &= state1->_marking[i] == state2->_marking[i];
				if(!equal)
					return false;
			}
			return true;
		}
		equal_to(unsigned int places, unsigned int variables)
			: nPlaces(places), nVariables(variables) {}
	private:
		unsigned int nPlaces;
		unsigned int nVariables;
	};
	template<size_t blocksize> friend class StateAllocator;
	template<size_t blocksize> friend class BoundedStateAllocator;
private:
	State* _parent;
	unsigned int _parentTransition;
	MarkVal* _marking;
	VarVal* _valuation;
};

}}

#endif //GENERALSTATE_H
