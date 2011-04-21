#ifndef HEURISTICDFS_H
#define HEURISTICDFS_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/DistanceMatrix.h"
#include "../PQL/Contexts.h"

namespace PetriEngine {
namespace Reachability {

/** Heuristic Depth First Reachability Search */
class HeuristicDFS : public ReachabilitySearchStrategy
{
public:
	/** Create instance of HeuristicDFS
	 * @param distanceStrategy Distance strategy, for computation of distance to query
	 */
	HeuristicDFS(PQL::DistanceContext::DistanceStrategy distanceStrategy){
		_distanceStrategy = distanceStrategy;
	}
	/** Perform reachability check using random DFS with a hash table */
	ReachabilityResult reachable(const PetriNet& net,
								 const MarkVal* m0,
								 const VarVal* v0,
								 PQL::Condition* query);
private:
	PQL::DistanceContext::DistanceStrategy _distanceStrategy;
};

} // Reachability
} // PetriEngine

#endif // HEURISTICDFS_H
