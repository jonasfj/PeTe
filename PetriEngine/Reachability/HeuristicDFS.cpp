#include "HeuristicDFS.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/StateSet.h"
#include "../Structures/StateAllocator.h"

#include <list>
#include <string.h>
#include <stdlib.h>

using namespace PetriEngine::PQL;
using namespace PetriEngine::Structures;

namespace PetriEngine {
namespace Reachability {


ReachabilityResult HeuristicDFS::reachable(const PetriNet& net,
										   const MarkVal* m0,
										   const VarVal* v0,
										   PQL::Condition* query){
	//Do we initially satisfy query?
	if(query->evaluate(PQL::EvaluationContext(m0, v0)))
		return ReachabilityResult(ReachabilityResult::Satisfied,
								  "A state satisfying the query was found");

	StateSet states(net);
	StateAllocator<> allocator(net);
	std::list<State*> stack;

	State* s0 = allocator.createState();
	memcpy(s0->marking(), m0, sizeof(MarkVal)*net.numberOfPlaces());
	memcpy(s0->valuation(), v0, sizeof(VarVal)*net.numberOfVariables());
	stack.push_back(s0);
	states.add(s0);

	Structures::DistanceMatrix distanceMatrix(net);

	State* ns = allocator.createState();
	while(!stack.empty()){
		State* s = stack.back();
		stack.pop_back();
		State* succ[net.numberOfTransitions()];
		double distances[net.numberOfTransitions()];
		memset(succ, 0, net.numberOfTransitions()*sizeof(State*));
		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s, ns)){
				if(states.add(ns)){
					if(query->evaluate(*ns))
						return ReachabilityResult(ReachabilityResult::Satisfied,
												"A state satisfying the query was found");
					ns->setParent(s);
					ns->setTransition(t);
					PQL::DistanceContext context(net,
												 _distanceStrategy,
												 ns->marking(),
												 ns->valuation(),
												 &distanceMatrix);
					succ[t] = ns;
					distances[t] = query->distance(context);
					ns = allocator.createState();
				}
			}
		}

		// Pretty bobble sort of the successors
		while(true){
			bool foundSomething = false;
			unsigned int min = 0;
			for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
				if((foundSomething = succ[t] && (!foundSomething || distances[t] < distances[min])))
					min = t;
			}
			if(foundSomething){
				stack.push_back(succ[min]);
				succ[min] = NULL;
			}else
				break;
		}
	}

	return ReachabilityResult(ReachabilityResult::NotSatisfied,
						"No state satisfying the query exists.");
}

} // Reachability
} // PetriEngine
