#ifndef DEPTHFIRSTREACHABILITYSEARCH_H
#define DEPTHFIRSTREACHABILITYSEARCH_H

#include "ReachabilitySearchStrategy.h"
#include "CoverabilityTreeNode.h"

namespace PetriEngine { namespace Reachability {

/** Implements reachability check in a DFS manner */
class DepthFirstReachabilitySearch : public ReachabilitySearchStrategy
{
public:
	DepthFirstReachabilitySearch() : ReachabilitySearchStrategy(){
		this->_coverabilityTree = NULL;
	}

	bool reachable(const PetriNet &net,
				   const MarkVal* initialMarking,
				   const VarVal* initialAssignment,
				   PQL::Condition* query);
	bool reachable(CoverabilityTreeNode* tree,
				   const PetriNet &net,
				   const MarkVal* initialMarking,
				   const VarVal* initialAssignment,
				   PQL::Condition* query);
	void setProgressReporter(ProgressReporter* reporter);
	/** The coverability tree of the net */
	const CoverabilityTreeNode* coverabilityTree();
private:
	CoverabilityTreeNode* _coverabilityTree;
};
} // Reachability
} // PetriEngine
#endif // DEPTHFIRSTREACHABILITYSEARCH_H
