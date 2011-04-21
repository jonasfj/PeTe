#include "BestFirstReachabilityStrategy.h"
#include "../PQL/PQL.h"
#include "../Structures/PriorityQueue.h"
#include "../Structures/StateSet.h"
#include "../Structures/StateAllocator.h"

#include <string.h>

using namespace PetriEngine::Structures;
using namespace PetriEngine::PQL;

namespace PetriEngine{
namespace Reachability{

ReachabilityResult BestFirstReachabilityStrategy::reachable(const PetriNet &net,
															const MarkVal *m0,
															const VarVal *v0,
															PQL::Condition *query){
	StateAllocator<> allocator(net);

	State* s0 = allocator.createState();
	memcpy(s0->marking(), m0, sizeof(MarkVal) * net.numberOfPlaces());
	memcpy(s0->valuation(), v0, sizeof(VarVal) * net.numberOfVariables());

	if(query->evaluate(*s0))
		return ReachabilityResult(ReachabilityResult::Satisfied, "Satisfied initially");

	//Initialize subclasses
	initialize(query, net);

	StateSet states(net);
	states.add(s0);
	PriorityQueue<State*> queue;
	queue.push(0, s0);

	//Allocate new state
	State* ns = allocator.createState();
	State* ns2 = allocator.createState();
	State* ns3 = allocator.createState();
	int count = 0;
	size_t max = 1;
	while(!queue.empty()){
		if(count++ & 1<<17){
			count = 0;
			if(queue.size() > max)
				max = queue.size();
			this->reportProgress((double)(max - queue.size()) / ((double)(max)));
			if(this->abortRequested())
				return ReachabilityResult(ReachabilityResult::Unknown,
										  "Query aborted!");
		}

		//Take something out of the queue
		PriorityQueue<State*>::Iterator it = queue.pop();
		if(!it++) break;	//Assume there's a first element
		State* s = it.item();
		queue.remove(it);	//Remove the element

		// Attempt to fire each transition
		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s->marking(), s->valuation(), ns->marking(), ns->valuation())){
				//If it's new
				if(states.add(ns)){
					//Set parent and transition for the state
					ns->setParent(s);
					ns->setTransition(t);

					//Test query
					if(query->evaluate(*ns))
						return ReachabilityResult(ReachabilityResult::Satisfied,
												  "Query was satified!");

					// Insert in queue, with given priority
					double bestp = priority(ns, query, net);
					queue.push(bestp, ns);


					if(fireUntillNoBetter && net.fire(t, ns, ns2)){
						if(query->evaluate(*ns2))
							return ReachabilityResult(ReachabilityResult::Satisfied,
												  "Query was satified!");
						double p = priority(ns2, query, net);
						if(p <= bestp){
							bestp = p;
							while(net.fire(t, ns2, ns3) && (p = priority(ns3, query, net)) <= bestp){
								bestp = p;
								State* tmp = ns2;	//SWAP ns2 and ns3
								ns2 = ns3;
								ns3 = tmp;
								if(query->evaluate(*ns2))
									return ReachabilityResult(ReachabilityResult::Satisfied,
													  "Query was satified!");
							}
							if(states.add(ns2)){
								queue.push(priority(ns2, query, net), ns2);
								ns2 = allocator.createState();
							}
						}
					}

					//Allocate new stake, as states take ownership
					ns = allocator.createState();
				}
			}
		}
	}

	return ReachabilityResult(ReachabilityResult::NotSatisfied,
							  "Query cannot be satisfied!");
}

//Empty default implementation
void BestFirstReachabilityStrategy::initialize(const PQL::Condition*, const PetriNet&){}

} // Reachability
} // PetriEngine
