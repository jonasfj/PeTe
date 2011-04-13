#ifndef STATESET_H
#define STATESET_H

#include <tr1/unordered_set>
#include <iostream>
#include "State.h"

namespace PetriEngine { namespace Reachability {

class StateSet: public std::tr1::unordered_set<State*, State::hash, State::equal_to>{
public:
	StateSet(const PetriNet& net)
		: std::tr1::unordered_set<State*, State::hash, State::equal_to>
			(8, State::hash(net.numberOfPlaces(), net.numberOfVariables()),
			 State::equal_to(net.numberOfPlaces(),net.numberOfVariables()))
		{}
	StateSet(unsigned int places, unsigned int variables)
		: std::tr1::unordered_set<State*, State::hash, State::equal_to>
			(8, State::hash(places, variables),
			 State::equal_to(places, variables))
		{}
	~StateSet(){
		for(iter it = this->begin(); it != this->end(); it++)
			State::deleteState(*it);
	}
	bool add(State* state) {
		std::pair<iter, bool> result = this->insert(state);
		return result.second;
	}

private:
	typedef std::tr1::unordered_set<State*, State::hash, State::equal_to>::const_iterator const_iter;
	typedef std::tr1::unordered_set<State*, State::hash, State::equal_to>::iterator iter;
};

}}


#endif // STATESET_H
