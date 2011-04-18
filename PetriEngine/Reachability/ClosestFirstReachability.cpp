#include "ClosestFirstReachability.h"
#include "../PQL/Contexts.h"

namespace PetriEngine{
namespace Reachability{

double ClosestFirstReachability::priority(const Structures::State *state,
												const PQL::Condition *query,
												const PetriNet& net){
	PQL::DistanceContext context(net,
								 _distanceStrategy,
								 state->marking(),
								 state->valuation());
	return query->distance(context);
}

} // Reachability
} // PetriEngine
