#ifndef GENERALSTATE_H
#define GENERALSTATE_H

#include "../PetriNet.h"
#include <stdlib.h>

namespace PetriEngine { namespace Reachability {

/** GeneralState class for reachability searches.
  * Used in most reachability search cases */
class State {
public:
	/** Getter for the marking */
	MarkVal* marking(){
		return _marking;
	}
	/** Getter for the valuation */
	VarVal* valuation(){
		return _valuation;
	}
	/** Getter for the parent */
	State* parent(){
		return _parent;
	}
	/** Getter for the transition the parent took to get here */
	unsigned int transition(){
		return _parentTransition;
	}
	/** Setter for the transition the parent took to get here */
	void setTransition(unsigned int t){
		_parentTransition = t;
	}
	/** Check if this state is a loop */
	bool isLoop(const PetriNet& net){

		State* parent = this->_parent;

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

	/*** Static and non-mission-critical functionality ***/

	/** Create a new state */
	static inline State* createState(int nPlaces, int nVars, State* parent = NULL) {
		char *d = (char*)calloc(1, sizeof(State) + sizeof(MarkVal)*nPlaces + sizeof(VarVal)*nVars);
		State* s = (State*)d;
		s->_parentTransition = 0;
		s->_marking = (MarkVal*)(d + sizeof(State));
		s->_valuation = (VarVal*)(d+ sizeof(State) + sizeof(MarkVal) * nPlaces);
		if(parent)
			s->_parent = parent;
		return (State*)d;
	}

	/** Create a new state from a net */
	static inline State* createState(const PetriNet& net, State* parent = NULL){
		char *d = (char*)calloc(1, sizeof(State) + sizeof(MarkVal)*net.numberOfPlaces() + sizeof(VarVal)*net.numberOfVariables());
		State* s = (State*)d;
		s->_parentTransition = 0;
		s->_marking = (MarkVal*)(d + sizeof(State));
		s->_valuation = (VarVal*)(d+ sizeof(State) + sizeof(MarkVal) * net.numberOfVariables());
		if(parent)
			s->_parent = parent;
		return (State*)d;
	}

	/** Deletes a state */
	static inline void deleteState(State* state){
		free(state);
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

private:
	State* _parent;
	unsigned int _parentTransition;
	MarkVal* _marking;
	VarVal* _valuation;
};

}}

#endif //GENERALSTATE_H
