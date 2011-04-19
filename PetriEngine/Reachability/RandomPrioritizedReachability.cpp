#include "RandomPrioritizedReachability.h"

#include <stdlib.h>

//Random number chosen by fair dice roll
#define SEED		1337

namespace PetriEngine {
namespace Reachability {

RandomPrioritizedReachability::RandomPrioritizedReachability(){
}

void RandomPrioritizedReachability::initialize(const PQL::Condition *,
											   const PetriNet &){
	srand(SEED);
}

double RandomPrioritizedReachability::priority(const Structures::State *state,
											   const PQL::Condition *query,
											   const PetriNet &net){
	return (double)rand() / RAND_MAX;
}

} // Reachability
} // PetriEngine
