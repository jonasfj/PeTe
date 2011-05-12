#ifndef ULTIMATESEARCH_H
#define ULTIMATESEARCH_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/State.h"
#include "../PQL/PQL.h"

namespace PetriEngine{
namespace Reachability{

/** Ultimate Search strategy, combination of the existing */
class UltimateSearch : public ReachabilitySearchStrategy
{
public:
	ReachabilityResult reachable(const PetriNet &net,
								 const MarkVal* m0,
								 const VarVal* v0,
								 PQL::Condition* query);
};

} // Reachability
} // PetriEngine

#endif // ULTIMATESEARCH_H
