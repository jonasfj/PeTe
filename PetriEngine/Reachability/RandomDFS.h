#ifndef RANDOMDFS_H
#define RANDOMDFS_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/State.h"

using namespace PetriEngine::Structures;

namespace PetriEngine { namespace Reachability {

/** Implements reachability check in a random DFS manner, using a hash table */
class RandomDFS : public ReachabilitySearchStrategy {
public:
	RandomDFS() : ReachabilitySearchStrategy(){}

	/** Perform reachability check using random DFS with a hash table */
	ReachabilityResult reachable(const PetriNet& net,
								 const MarkVal* m0,
								 const VarVal* v0,
								 PQL::Condition* query);
};

}}	// Namespaces

#endif // RANDOMDFS_H
