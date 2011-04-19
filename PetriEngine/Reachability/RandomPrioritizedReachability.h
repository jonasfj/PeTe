#ifndef RANDOMPRIORITIZEDREACHABILITY_H
#define RANDOMPRIORITIZEDREACHABILITY_H

#include "BestFirstReachabilityStrategy.h"

namespace PetriEngine {
namespace Reachability {

/** Reachability strategy that prioritizes randomly */
class RandomPrioritizedReachability : public BestFirstReachabilityStrategy
{
public:
    RandomPrioritizedReachability();
private:
	void initialize(const PQL::Condition *query, const PetriNet &net);
	double priority(const Structures::State *state,
					const PQL::Condition *query,
					const PetriNet &net);
};

} // Reachability
} // PetriEngine

#endif // RANDOMPRIORITIZEDREACHABILITY_H
