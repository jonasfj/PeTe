#ifndef CLOSESTFIRSTREACHABILITY_H
#define CLOSESTFIRSTREACHABILITY_H

#include "BestFirstReachabilityStrategy.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/DistanceMatrix.h"

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
	ClosestFirstReachability(PQL::DistanceContext::DistanceStrategy distanceStrategy){
		_distanceStrategy = distanceStrategy;
		_dm = NULL;
	}
private:
	void initialize(const PQL::Condition* query, const PetriNet& net);
	double priority(const Structures::State *state,
					const PQL::Condition *query,
					const PetriNet &net);
	PQL::DistanceContext::DistanceStrategy _distanceStrategy;
	Structures::DistanceMatrix* _dm;
};

} // Reachability
} // PetriEngine

#endif // CLOSESTFIRSTREACHABILITY_H
