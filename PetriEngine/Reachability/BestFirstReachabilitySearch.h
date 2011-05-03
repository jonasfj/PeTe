#ifndef BESTFIRSTREACHABILITYSTRATEGY_H
#define BESTFIRSTREACHABILITYSTRATEGY_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/State.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/DistanceMatrix.h"


namespace PetriEngine{

namespace Structures{
	class StateSet;
}

namespace Reachability{


/** Best first reachability strategy */
class BestFirstReachabilitySearch : public ReachabilitySearchStrategy
{
public:
	/** Create instance of BestFirstReachability
	 * @param distanceStrategy Distance strategy, for computation of distance to query
	 */
	BestFirstReachabilitySearch(PQL::DistanceContext::DistanceStrategy distanceStrategy,
								bool fireUntillNoBetter = false,
								bool depthFirst = true,
								int lookahead = 0){
		this->_distanceStrategy = distanceStrategy;
		this->fireUntillNoBetter = fireUntillNoBetter;
		this->depthFirst = depthFirst;
		_lookahead = lookahead;
	}
	ReachabilityResult reachable(const PetriNet &net,
								 const MarkVal* m0,
								 const VarVal* v0,
								 PQL::Condition* query);
private:
	void initialize(const PQL::Condition* query, const PetriNet& net);
	/** Method for prioritizing different states, lower priority is better */
	double priority(const Structures::State* state,
					const PQL::Condition* query,
					const PetriNet& net);
	PQL::DistanceContext::DistanceStrategy _distanceStrategy;
	Structures::DistanceMatrix* _dm;
	bool fireUntillNoBetter;
	int _lookahead;
	bool depthFirst;

	/** Find the best distance in depth steps from state */
	double lookaheadDistance(const PetriNet& net,
							 const Structures::State* state,
							 const PQL::Condition* query,
							 int depth);
	Structures::StateSet* _states;
};

} // Reachability
} // PetriEngine

#endif // BESTFIRSTREACHABILITYSTRATEGY_H
