#ifndef DEPTHFIRSTREACHABILITYSEARCH_H
#define DEPTHFIRSTREACHABILITYSEARCH_H

#include "ReachabilitySearchStrategy.h"
#include "CoverabilityTreeNode.h"

namespace PetriEngine { namespace Reachability {

/** Implements reachability check in a DFS manner */
class DepthFirstReachabilitySearch : public ReachabilitySearchStrategy
{
public:
	DepthFirstReachabilitySearch() : ReachabilitySearchStrategy(),
									 _coverabilityTree(NULL, NULL){
		_reporter = NULL;
	}

	/** Perform reachability check using the DepthFirstSearch */
	bool reachable(const PetriNet &net,
				   const MarkVal* initialMarking,
				   const VarVal* initialAssignment,
				   PQL::Condition* query);

	void setProgressReporter(ProgressReporter* reporter);
private:
	/** Internal reachability method */
	bool dfsReachable(CoverabilityTreeNode* tree,
				   const PetriNet &net,
				   const MarkVal* initialMarking,
				   const VarVal* initialAssignment,
				   PQL::Condition* query);

	/** Root node of the reachability search */
	CoverabilityTreeNode _coverabilityTree;

	/** The report to report progress too */
	ProgressReporter* _reporter;
};
} // Reachability
} // PetriEngine
#endif // DEPTHFIRSTREACHABILITYSEARCH_H
