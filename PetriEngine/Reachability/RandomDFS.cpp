#include "RandomDFS.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/StateSet.h"
#include "../Structures/StateAllocator.h"

#include <list>
#include <string.h>
#include <stdlib.h>
#include <time.h>

using namespace PetriEngine::PQL;
using namespace PetriEngine::Structures;

namespace PetriEngine{ namespace Reachability {

ReachabilityResult RandomDFS::reachable(const PetriNet &net,
										const MarkVal *m0,
										const VarVal *v0,
										PQL::Condition *query){
	//Do we initially satisfy query?
	if(query->evaluate(PQL::EvaluationContext(m0, v0)))
		return ReachabilityResult(ReachabilityResult::Satisfied,
								  "A state satisfying the query was found", 0, 0);

	StateSet states(net);
	StateAllocator<> allocator(net);
	std::list<State*> stack;
	srand(time(0));	// Chosen by fair dice roll

	State* s0 = allocator.createState();
	memcpy(s0->marking(), m0, sizeof(MarkVal)*net.numberOfPlaces());
	memcpy(s0->valuation(), v0, sizeof(VarVal)*net.numberOfVariables());

	stack.push_back(s0);
	states.add(s0);
	State* ns = allocator.createState();

	unsigned int max = 0;
	int count = 0;
	BigInt exploredStates = 0;
	BigInt expandedStates = 0;
	while(!stack.empty()){
		// Progress reporting and abort checking
		if(count++ & 1<<17){
			if(stack.size() > max)
				max = stack.size();
			count = 0;
			// Report progress
			reportProgress((double)(max - stack.size())/(double)max);
			// Check abort
			if(abortRequested())
				return ReachabilityResult(ReachabilityResult::Unknown,
										"Search was aborted.");
		}

		State* s = stack.back();
		stack.pop_back();
		State* succ[net.numberOfTransitions()];
		memset(succ, 0, net.numberOfTransitions()*sizeof(State*));
		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s, ns)){
				if(states.add(ns)){
					exploredStates++;
					if(query->evaluate(*ns))
						return ReachabilityResult(ReachabilityResult::Satisfied,
												"A state satisfying the query was found", expandedStates, exploredStates);
					ns->setParent(s);
					ns->setTransition(t);
					succ[t] = ns;
					ns = allocator.createState();
				}
			}
		}
		// Randomly sorts states into the stack
		expandedStates++;
		int random;
		int t;
		do {
			random = rand() % net.numberOfTransitions();
			t = random;
			do{
				if(succ[t]){
					stack.push_back(succ[t]);
					succ[t] = NULL;
					t++;
					break;
				}
				t = (t+1) % net.numberOfTransitions();
			} while(t != random);
		} while(t != random);
	}

	return ReachabilityResult(ReachabilityResult::NotSatisfied,
						"No state satisfying the query exists.", expandedStates, exploredStates);
}

}} // Namespaces
