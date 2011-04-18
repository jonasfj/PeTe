#include "HashUnderApproximation.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/StateSet.h"
#include <iostream>
#include <list>
#include <string.h>

using namespace PetriEngine::Structures;

typedef std::tr1::unordered_set<size_t> HashSet;
typedef std::tr1::unordered_set<size_t>::iterator HashSetIter;

namespace PetriEngine { namespace Reachability {

ReachabilityResult HashUnderApproximation::reachable(const PetriNet &net,
													 const MarkVal *m0,
													 const VarVal *v0,
													 PQL::Condition *query){

	// Check for initial satisfying query
	if(query->evaluate(PQL::EvaluationContext(m0,v0)))
		return ReachabilityResult(ReachabilityResult::Satisfied,
								  "A state satisfying the query was found");

	HashSet states;
	std::list<Step> stack;

	State* s0 = State::createState(net.numberOfPlaces(),net.numberOfVariables());
	memcpy(s0->marking(),m0,sizeof(MarkVal)*net.numberOfPlaces());
	memcpy(s0->valuation(),v0,sizeof(VarVal)*net.numberOfVariables());
	stack.push_back(Step(s0,0));

	State::hash hasher(net.numberOfPlaces(),net.numberOfVariables());

	unsigned int max =0;
	int count = 0;
	// Main loop
	while(!stack.empty()){
		// Magic number, so we do not report progress too often
		if(count++ & 1<<17){
			if(stack.size()>max)
				max=stack.size();
			count=0;
			//report progress
			reportProgress((double)(max-stack.size())/(double)max);
			//check abort
			if(abortRequested())
				return ReachabilityResult(ReachabilityResult::Unknown,
										  "Search was aborted");
		}

		//Take first step of the stack
		State* s = stack.back().state;
		State* ns = State::createState(net.numberOfPlaces(),net.numberOfVariables(), s);
		bool foundSomething = false;
		for(unsigned int t = stack.back().t; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s->marking(), s->valuation(), ns->marking(), ns->valuation())){
				std::pair<HashSetIter,bool> result = states.insert(hasher(ns));
				if(result.second){
					ns->setTransition(t);
					if(query->evaluate(PQL::EvaluationContext(ns->marking(), ns->valuation())))
						return ReachabilityResult(ReachabilityResult::Satisfied,
									  "A state satisfying the query was found");
					stack.back().t = t + 1;
					stack.push_back(Step(ns, 0));
					foundSomething = true;
					break;
				}
			}
		}
		if(!foundSomething){
			State::deleteState(s);
			State::deleteState(ns);
			ns = NULL;
			stack.pop_back();
		}
	}
	return ReachabilityResult(ReachabilityResult::Unknown,
							"Could not disprove the existence of a state not satisfying the query.");
}

}}
