#ifndef DEPTHFIRSTREACHABILITYSEARCH_H
#define DEPTHFIRSTREACHABILITYSEARCH_H

#include "ReachabilitySearchStrategy.h"

namespace PetriEngine {

/** Implements reachability check in a DFS manner */
class DepthFirstReachabilitySearch : public ReachabilitySearchStrategy
{
public:
	bool reachable(PetriNet net, Mark* initialMarking);
	void setProgressReporter(ProgressReporter* reporter);
};

} // PetriEngine
#endif // DEPTHFIRSTREACHABILITYSEARCH_H
