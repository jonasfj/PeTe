#ifndef KARPMILLERL1SEARCHSTRATEGY_H
#define KARPMILLERL1SEARCHSTRATEGY_H

#include "ReachabilitySearchStrategy.h"

#include <stdint.h>

namespace PetriEngine{ namespace Reachability{

/** An implementation of Karp-Miller using L1 cache (very little memory) */
class KarpMillerL1SearchStrategy : public ReachabilitySearchStrategy
{
public:
    KarpMillerL1SearchStrategy();
	bool reachable(const PetriNet &net, const MarkVal *initialMarking, PQL::Condition *query);
private:
	/** Attempt to fire a transition */
	inline bool fire(const PetriNet& net, uint8_t t, const MarkVal* old_m, MarkVal* new_m);
};

} // Reachability
} // PetriEngine

#endif // KARPMILLERL1SEARCHSTRATEGY_H
