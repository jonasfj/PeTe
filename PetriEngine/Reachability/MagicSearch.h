#ifndef MAGICSEARCH_H
#define MAGICSEARCH_H

#include "ReachabilitySearchStrategy.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/DistanceMatrix.h"
#include "../Structures/SmartState.h"
#include "../Structures/SmartStateSet.h"
#include "../Structures/SmartStateAllocator.h"

namespace PetriEngine{
namespace Reachability{

/** Magic combination of reachability strategies */
class MagicSearch : public ReachabilitySearchStrategy
{
	/** Step in the queue */
	struct Step{
		Structures::SmartState* state;
		int lastStored, lastApprox, depth;
		//TODO: Add some notation of firing vector
	};
public:
	/** Create instance of BestFirstReachability
	 * @param distanceStrategy Distance strategy, for computation of distance to query
	 */
	MagicSearch(PQL::DistanceContext::DistanceStrategy distanceStrategy,
				bool depthFirst = true){
		this->_distanceStrategy = distanceStrategy;
		this->depthFirst = depthFirst;
	}
	ReachabilityResult reachable(const PetriNet &net,
								 const MarkVal* m0,
								 const VarVal* v0,
								 PQL::Condition* query);
private:
	PQL::DistanceContext::DistanceStrategy _distanceStrategy;
	Structures::DistanceMatrix* _dm;
	bool depthFirst;
	/** Method for prioritizing different states, lower priority is better */
	double priority(const MarkVal* marking,
					const VarVal* valuation,
					const PQL::Condition* query,
					const PetriNet& net);
};

} // Reachability
} // PetriEngine

#endif // MAGICSEARCH_H
