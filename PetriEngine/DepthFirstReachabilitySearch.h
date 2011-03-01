#ifndef DEPTHFIRSTREACHABILITYSEARCH_H
#define DEPTHFIRSTREACHABILITYSEARCH_H

#include "ReachabilitySearchStrategy.h"
#include "CoverabilityTreeNode.h"

namespace PetriEngine {

/** Implements reachability check in a DFS manner */
class DepthFirstReachabilitySearch : public ReachabilitySearchStrategy
{
public:
	DepthFirstReachabilitySearch() : ReachabilitySearchStrategy(){
		this->_coverabilityTree = NULL;
	}

	bool reachable(PetriNet net, Mark* initialMarking);
	void setProgressReporter(ProgressReporter* reporter);
	/** The coverability tree of the net */
	CoverabilityTreeNode* coverabilityTree();
private:
	CoverabilityTreeNode* _coverabilityTree;
};

} // PetriEngine
#endif // DEPTHFIRSTREACHABILITYSEARCH_H
