#ifndef DEPTHFIRSTREACHABILITYSEARCH_H
#define DEPTHFIRSTREACHABILITYSEARCH_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/State.h"

using namespace PetriEngine::Structures;

namespace PetriEngine { namespace Reachability {

/** Implements reachability check in a DFS manner */
class DepthFirstReachabilitySearch : public ReachabilitySearchStrategy
{
public:
	DepthFirstReachabilitySearch() : ReachabilitySearchStrategy(){}

	/** Perform reachability check using the DepthFirstSearch */
	ReachabilityResult reachable(const PetriNet &net,
								 const MarkVal* initialMarking,
								 const VarVal* initialAssignment,
								 PQL::Condition* query);
private:
	/** A step in the reachability search */
	struct Step{
		Step(Structures::State* s, unsigned int t){
			state = s;
			this->t = t;
		}
		Structures::State* state;
		unsigned int t;
	};
};
} // Reachability
} // PetriEngine
#endif // DEPTHFIRSTREACHABILITYSEARCH_H
