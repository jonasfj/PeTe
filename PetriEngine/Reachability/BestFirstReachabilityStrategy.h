#ifndef BESTFIRSTREACHABILITYSTRATEGY_H
#define BESTFIRSTREACHABILITYSTRATEGY_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/State.h"

namespace PetriEngine{
namespace Reachability{

/** Best first reachability strategy */
class BestFirstReachabilityStrategy : public ReachabilitySearchStrategy
{
public:
	ReachabilityResult reachable(const PetriNet &net,
								 const MarkVal* m0,
								 const VarVal* v0,
								 PQL::Condition* query);
private:
	virtual unsigned int priority(const State* state,
								  const PQL::Condition* query,
								  const PetriNet& net) = 0;
};

} // Reachability
} // PetriEngine

#endif // BESTFIRSTREACHABILITYSTRATEGY_H
