#include "BreadthFirstReachabilitySearch.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/StateSet.h"
#include "../Structures/StateAllocator.h"

#include <list>
#include <string.h>

using namespace PetriEngine::PQL;
using namespace PetriEngine::Structures;

namespace PetriEngine{ namespace Reachability {

ReachabilityResult BreadthFirstReachabilitySearch::reachable(const PetriNet &net,
															 const MarkVal *m0,
															 const VarVal *v0,
															 PQL::Condition *query){
	//Do we initially satisfy query?
	if(query->evaluate(PQL::EvaluationContext(m0, v0)))
		return ReachabilityResult(ReachabilityResult::Satisfied,
								  "A state satisfying the query was found");
	//Create StateSet
	StateSet states(net);
	std::list<State*> queue;

	StateAllocator<1000000> allocator(net);

	State* s0 = allocator.createState();
	memcpy(s0->marking(), m0, sizeof(MarkVal)*net.numberOfPlaces());
	memcpy(s0->valuation(), v0, sizeof(VarVal)*net.numberOfVariables());

	queue.push_back(s0);

	unsigned int max = 0;
	int count = 0;
	State* ns = allocator.createState();
	while(!queue.empty()){
		// Progress reporting and abort checking
		if(count++ & 1<<17){
			if(queue.size() > max)
				max = queue.size();
			count = 0;
			// Report progress
			reportProgress((double)(max - queue.size())/(double)max);
			// Check abort
			if(abortRequested())
				return ReachabilityResult(ReachabilityResult::Unknown,
										"Search was aborted.");
		}

		State* s = queue.front();
		queue.pop_front();
		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s, ns)){
				if(states.add(ns)){
					if(query->evaluate(*ns))
						return ReachabilityResult(ReachabilityResult::Satisfied,
												"A state satisfying the query was found");
					ns->setParent(s);
					ns->setTransition(t);
					queue.push_back(ns);
					ns = allocator.createState();
				}
			}
		}
	}

	return ReachabilityResult(ReachabilityResult::NotSatisfied,
						"No state satisfying the query exists.");
}

}}
