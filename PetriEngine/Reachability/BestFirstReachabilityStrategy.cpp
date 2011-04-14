#include "BestFirstReachabilityStrategy.h"
#include "../PQL/PQL.h"

using namespace PetriEngine::Structures;
using namespace PetriEngine::PQL;

namespace PetriEngine{
namespace Reachability{

ReachabilityResult BestFirstReachabilityStrategy::reachable(const PetriNet &net,
															const MarkVal *m0,
															const VarVal *v0,
															PQL::Condition *query){
	State* s0 = State::createState(net);
	memcpy(s0->marking(), m0, sizeof(MarkVal) * net.numberOfPlaces());
	memcpy(s0->valuation(), v0, sizeof(VarVal) * net.numberOfVariables());

	if(query->evaluate(*s0))
		return ReachabilityResult(ReachabilityResult::Satisfied, "Satisfied initially");
}

} // Reachability
} // PetriEngine
