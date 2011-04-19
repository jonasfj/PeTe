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
								 state->valuation(),
								 _dm);
	return query->distance(context);
}

void ClosestFirstReachability::initialize(const PQL::Condition*,
										  const PetriNet& net){
	_dm = new Structures::DistanceMatrix(net);
}

} // Reachability
} // PetriEngine
