#ifndef BREADTHFIRSTREACHABILITYSEARCH_H
#define BREADTHFIRSTREACHABILITYSEARCH_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/State.h"

namespace PetriEngine { namespace Reachability {

/** Implements reachability check in a BFS manner using a hash table */
class BreadthFirstReachabilitySearch : public ReachabilitySearchStrategy {
public:
	BreadthFirstReachabilitySearch() : ReachabilitySearchStrategy() {}

	/** Perform reachability check using BFS with hasing */
	ReachabilityResult reachable(const PetriNet &net,
								 const MarkVal *m0,
								 const VarVal *v0,
								 PQL::Condition *query);
};

}} // Namespaces

#endif // BREADTHFIRSTREACHABILITYSEARCH_H
