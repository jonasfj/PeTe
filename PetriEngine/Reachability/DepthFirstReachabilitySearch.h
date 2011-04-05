#ifndef DEPTHFIRSTREACHABILITYSEARCH_H
#define DEPTHFIRSTREACHABILITYSEARCH_H

#include <stdlib.h>
#include <string.h>

#include "ReachabilitySearchStrategy.h"
#include "GeneralState.h"

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

	void setProgressReporter(ProgressReporter* reporter);
private:
	typedef GenericState State;
	/** Internal reachability method */
	bool dfsReachable(State* oldStates,
				   const PetriNet &net,
				   const MarkVal* initialMarking,
				   const VarVal* initialAssignment,
				   PQL::Condition* query);

	/** The report to report progress too */
	ProgressReporter* _reporter;

};
} // Reachability
} // PetriEngine
#endif // DEPTHFIRSTREACHABILITYSEARCH_H
