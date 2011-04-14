#ifndef CLOSESTFIRSTREACHABILITY_H
#define CLOSESTFIRSTREACHABILITY_H

#include "BestFirstReachabilityStrategy.h"
#include "../PQL/PQL.h"

namespace PetriEngine{
namespace Reachability{

/** Closest first reachability search strategy
 * @remarks Strategy that expands closests states first
 */
class ClosestFirstReachability : public BestFirstReachabilityStrategy
{
public:
	/** Create instance of ClosestFirstReachability
	 * @param distanceStrategy Distance strategy, for computation of distance to query
	 */
	ClosestFirstReachability(PQL::Condition::DistanceStrategy distanceStrategy){
		_distanceStrategy = distanceStrategy;
	}
private:
	double priority(const Structures::State *state,
					const PQL::Condition *query,
					const PetriNet &net);
	PQL::Condition::DistanceStrategy _distanceStrategy;
};

} // Reachability
} // PetriEngine

#endif // CLOSESTFIRSTREACHABILITY_H
