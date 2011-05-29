/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef GENERALSTATE_H
#define GENERALSTATE_H

#include "../PetriNet.h"

#include <stdlib.h>
#include <stdio.h>
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

	/** Dump trace to stderr */
	void dumpTrace(const PetriNet& net){
		fprintf(stderr, "trace: (Latest first)");
		State* c = this;
		while(c->parent()){
			fprintf(stderr, "%s\n", net.transitionNames()[c->transition()].c_str());
			c = c->parent();
		}
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
