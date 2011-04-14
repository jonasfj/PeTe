#include "ClosestFirstReachability.h"

namespace PetriEngine{
namespace Reachability{

double ClosestFirstReachability::priority(const Structures::State *state,
												const PQL::Condition *query,
												const PetriNet&){
	PQL::EvaluationContext context(state->marking(), state->valuation());
	return query->distance(context, _distanceStrategy);
}

} // Reachability
} // PetriEngine
