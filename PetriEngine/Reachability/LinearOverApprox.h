#ifndef LINEAROVERAPPROX_H
#define LINEAROVERAPPROX_H

#include "ReachabilitySearchStrategy.h"

namespace PetriEngine{
namespace Reachability{

class LinearOverApprox : public ReachabilitySearchStrategy
{
public:
	LinearOverApprox(ReachabilitySearchStrategy* fallback = NULL){
		this->fallback = fallback;
	}
	ReachabilityResult reachable(const PetriNet &net, const MarkVal *initialMarking, const VarVal *initialAssignment, PQL::Condition *query);
private:
	ReachabilitySearchStrategy* fallback;
};

} // Reachability
} // PetriEngine

#endif // LINEAROVERAPPROX_H
