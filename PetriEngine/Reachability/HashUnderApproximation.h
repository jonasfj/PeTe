#ifndef HASHUNDERAPPROXIMATION_H
#define HASHUNDERAPPROXIMATION_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/State.h"

namespace PetriEngine { namespace Reachability {

/** Implements reachability check with under approximation using hashing */
class HashUnderApproximation : public ReachabilitySearchStrategy {
public:
	HashUnderApproximation() : ReachabilitySearchStrategy(){}

	/** Perform reachability check using under-approximation */
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
		// Transition fired
		unsigned int t;
	};
};

}} // Reachability .. PetriEngine

#endif // HASHUNDERAPPROXIMATION_H
