#ifndef DFRSHASH_H
#define DFRSHASH_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/State.h"

namespace PetriEngine { namespace Reachability {

/** Implements reachability check in a DFS manner, using a hash table */
class DFRSHash : public ReachabilitySearchStrategy {
public:
	DFRSHash() : ReachabilitySearchStrategy(){}

	/** Perform reachability check using DFS with hash table */
	ReachabilityResult reachable(const PetriNet& net,
								 const MarkVal* initialMarking,
								 const VarVal* initialValuation,
								 PQL::Condition* query);
private:
	/** A step in the reachability search */
	struct Step{
		Step(Structures::State* s, unsigned int t){
			state = s;
			this->t = t;
		}
		Structures::State* state;
		unsigned int t;
	};
};

}} // Reachability .. PetriEngine

#endif // DFRSHASH_H
