#include "KarpMillerL1SearchStrategy.h"
#include "../PQL/PQLExpressions.h"
#include "../PetriNet.h"

#include <string.h>
#include <assert.h>

#define MAX_DEPTH			4096

namespace PetriEngine{ namespace Reachability{

inline bool KarpMillerL1SearchStrategy::fire(const PetriNet& net, uint8_t t, const MarkVal* old_m, MarkVal* new_m){
	for(size_t i = 0; i < net.numberOfPlaces(); i++){
		new_m[i] = old_m[i] + net.transitionVector(t)[i];
		if(new_m[i] < 0)
			return false;
	}
	return true;
}

KarpMillerL1SearchStrategy::KarpMillerL1SearchStrategy() {}

bool KarpMillerL1SearchStrategy::reachable(const PetriNet &net,
										   const MarkVal *initialMarking,
										   PQL::Condition *query){
	assert(net.numberOfVariables() == 0);
	if(net.numberOfVariables() > 0){
		//TODO: Return unknown, or could not be found
		return false;
	}

	const unsigned int nTransitions = net.numberOfTransitions(),
					   nPlaces = net.numberOfPlaces();
	MarkVal old_m[nPlaces];
	MarkVal new_m[nPlaces];
	memcpy(old_m, initialMarking, nPlaces * sizeof(MarkVal));
	size_t depth = 1;
	uint8_t stack[MAX_DEPTH];
	stack[0] = 0;
	uint8_t t = 0;
	while(true){
		// Invariant:
		// old_m is the current marking
		// stack[depth] is where the next transtion goes
		// t is the next transition to fire

		while(!fire(net, t, old_m, new_m)){
			while(++t = nTransitions){
				if(!(--depth))		//Pop the stack
					return false;	//Terminate algorithm, we're done now
				// Reverse transition on the stack
				for(size_t i = 0; i < nPlaces; i++)
					old_m[i] -= net.transitionVector(t)[i];
				t = stack[depth];
			}
		}

		//Test if query is satisfied
		if(query->evaluate(PQL::EvaluationContext(new_m, NULL)))
			return true;

		//Check if it's seen
		bool seen;
		size_t d = depth;
		while(--d > 0){
			seen = true;
			const MarkVal* tv = net.transitionVector(stack[d]);
			for(size_t i = 0; i < nPlaces; i++){
				old_m[i] -= tv[i];
				seen &= old_m[i] == new_m[i];
			}
			if(seen)
				break;
		}
		if(!seen){
			stack[depth++] = t;
			memcpy(old_m, new_m, nPlaces * sizeof(MarkVal));
			t = 0;
		}else{
			//Pop if needed as long as needed
			while(++t = nTransitions){
				if(!(--depth))		//Pop the stack
					return false;	//Terminate algorithm, we're done now
				// Reverse transition on the stack
				for(size_t i = 0; i < nPlaces; i++)
					old_m[i] -= net.transitionVector(t)[i];
				t = stack[depth];
			}
		}
	}
}

} // Reachability
} // PetriEngine
