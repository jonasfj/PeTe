#include "RandomPrioritizedReachability.h"

#include <stdlib.h>
#include <time.h>

namespace PetriEngine {
namespace Reachability {

RandomPrioritizedReachability::RandomPrioritizedReachability(){
}

void RandomPrioritizedReachability::initialize(const PQL::Condition *,
											   const PetriNet &){
	srand(time(0));
}

double RandomPrioritizedReachability::priority(const Structures::State *,
											   const PQL::Condition *,
											   const PetriNet &){
	return (double)rand() / RAND_MAX;
}

} // Reachability
} // PetriEngine
