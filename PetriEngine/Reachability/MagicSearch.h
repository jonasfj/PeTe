#ifndef MAGICSEARCH_H
#define MAGICSEARCH_H

#include "ReachabilitySearchStrategy.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/DistanceMatrix.h"
#include "../Structures/SmartState.h"
#include "../Structures/SmartStateSet.h"
#include "../Structures/SmartStateAllocator.h"
#include "../Structures/Scale.h"

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
				bool depthFirst,
				Structures::Scale approxScale,
				Structures::Scale storeScale){
		this->_distanceStrategy = distanceStrategy;
		this->depthFirst = depthFirst;
		this->approxScale = approxScale;
		this->storeScale = storeScale;
	}
	ReachabilityResult reachable(const PetriNet &net,
								 const MarkVal* m0,
								 const VarVal* v0,
								 PQL::Condition* query);
private:
	Structures::Scale approxScale, storeScale;
	PQL::DistanceContext::DistanceStrategy _distanceStrategy;
	Structures::DistanceMatrix* _dm;
	/** Contraints for over-approximation, zero if analysis isn't possible */
	PQL::ConstraintAnalysisContext::ConstraintSet contraints;
	bool depthFirst;
	/** Method for prioritizing different states, lower priority is better */
	double priority(const MarkVal* marking,
					const VarVal* valuation,
					const PQL::Condition* query,
					const PetriNet& net);
	bool canExcludeByOverApprox(const PetriNet& net, const MarkVal* m, const VarVal* v);
};

} // Reachability
} // PetriEngine

#endif // MAGICSEARCH_H
