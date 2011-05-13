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
#ifndef SMARTSTATESET_H
#define SMARTSTATESET_H

#include <tr1/unordered_set>

#include "SmartState.h"

namespace PetriEngine{
namespace Structures{

/** Hash set for a collection of SmartStates */
class SmartStateSet{
	/** SmartState specialisation of std::hash */
	class hash : public std::unary_function<SmartState*, size_t>{
	public:
		size_t operator()(const SmartState* state) const{
			const MarkVal* m;
			const VarVal* v;
			_set.getState(state, m, v);
			size_t hash = 0;
			for(unsigned int i = 0; i < _set._net.numberOfPlaces(); i++)
				hash ^=	(m[i] << ((i*2) % (sizeof(MarkVal)*8))) | (m[i] >> (32 - ((i*2) % (sizeof(MarkVal)*8))));
			for(unsigned int i = 0; i < _set._net.numberOfVariables(); i++)
				hash ^= (v[i] << ((i*2) % (sizeof(VarVal)*8))) | (v[i] >> (32 - ((i*2) % (sizeof(VarVal)*8))));
			return hash;
		}
		hash(SmartStateSet& set) : _set(set) {}
	private:
		SmartStateSet& _set;
	};

	/** State specialisation of std::equal_to */
	class equal_to : public std::binary_function<SmartState*, SmartState*, bool>{
	public:
		bool operator()(const SmartState* state1, const SmartState* state2) const{
			const MarkVal* m1;
			const VarVal* v1;
			_set.getState(state1, m1, v1);
			const MarkVal* m2;
			const VarVal* v2;
			_set.getState(state2, m2, v2);
			bool equal = true;
			//Check marking
			for(unsigned int i = 0; i < _set._net.numberOfPlaces(); i++){
				equal &= m1[i] == m2[i];
				if(!equal)
					return false;
			}
			//Check assignment
			for(unsigned int i = 0; i < _set._net.numberOfVariables(); i++){
				equal &= v1[i] == v2[i];
				if(!equal)
					return false;
			}
			return true;
		}
		equal_to(SmartStateSet& set) : _set(set) {}
	private:
		SmartStateSet& _set;
	};
	typedef std::tr1::unordered_set<SmartState*, SmartStateSet::hash, SmartStateSet::equal_to> Set;
	typedef Set::iterator Iter;
public:
	SmartStateSet(const PetriNet& net) : _net(net), set(10, hash(*this), equal_to(*this)){
		useFirst = true;
		cachedState1 = NULL;
		cachedState2 = NULL;
		cachedMarking1 = new MarkVal[net.numberOfPlaces()];
		cachedMarking2 = new MarkVal[net.numberOfPlaces()];
		cachedValuation1 = new VarVal[net.numberOfVariables()];
		cachedValuation2 = new VarVal[net.numberOfVariables()];
		constCachedState = NULL;
	}
	~SmartStateSet(){
		if(cachedMarking1){
			delete[] cachedMarking1;
			cachedMarking1 = NULL;
		}
		if(cachedMarking2){
			delete[] cachedMarking2;
			cachedMarking2 = NULL;
		}
		if(cachedValuation1){
			delete[] cachedValuation1;
			cachedValuation1 = NULL;
		}
		if(cachedValuation2){
			delete[] cachedValuation2;
			cachedValuation2 = NULL;
		}
	}
	/** Add a new state, returns true if state didn't exist */
	bool add(SmartState* state, const MarkVal* marking, const VarVal* valuation){
		//Set state reference
		constCachedState = state;
		constCachedMarking = marking;
		constCachedValuation = valuation;

		std::pair<Iter, bool> result = set.insert(state);
		return result.second;
	}
private:
	void getState(const SmartState* state, MarkVal const *& marking, VarVal const *& valuation){
		if(constCachedState == state){
			marking = constCachedMarking;
			valuation = constCachedValuation;
		}else if(cachedState1 == state){
			marking = cachedMarking1;
			valuation = cachedValuation1;
		}else if(cachedState2 == state){
			marking = cachedMarking2;
			valuation = cachedValuation2;
		}else if(useFirst){
			useFirst = !useFirst;
			state->getState(_net, cachedMarking1, cachedValuation1);
			marking = cachedMarking1;
			valuation = cachedValuation1;
		}else{
			useFirst = !useFirst;
			state->getState(_net, cachedMarking2, cachedValuation2);
			marking = cachedMarking2;
			valuation = cachedValuation2;
		}
	}
	const PetriNet& _net;

	SmartState* constCachedState;
	const MarkVal* constCachedMarking;
	const VarVal* constCachedValuation;

	bool useFirst;

	SmartState* cachedState1;
	MarkVal* cachedMarking1;
	VarVal* cachedValuation1;

	SmartState* cachedState2;
	MarkVal* cachedMarking2;
	VarVal* cachedValuation2;

	Set set;
};

} // Structures
} // PetriEngine

#endif // SMARTSTATESET_H
