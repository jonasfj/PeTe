#include "DFRSHash.h"
#include "../PQL/PQL.h"
#include <iostream>
#include <list>
#include <string.h>

namespace PetriEngine { namespace Reachability {


ReachabilityResult DFRSHash::reachable(const PetriNet &net,
									   const MarkVal *m0,
									   const VarVal *v0,
									   PQL::Condition *query){
	//Do we initially satisfy query?
	if(query->evaluate(PQL::EvaluationContext(m0, v0)))
		return ReachabilityResult(ReachabilityResult::Satisfied,
								  "A state satisfying the query was found");
	//Create StateSet
	StateSet states(net);
	std::list<Step> stack;

	State* s0 = State::createState(net);
	memcpy(s0->marking(), m0, sizeof(MarkVal)*net.numberOfPlaces());
	memcpy(s0->valuation(), v0, sizeof(VarVal)*net.numberOfVariables());

	stack.push_back(Step(s0, 0));

	unsigned int max = 0;
	int count = 0;
	while(!stack.empty()){
		if(count++ & 1<<16){
			if(stack.size() > max)
				max = stack.size();
			count = 0;
			//report progress
			reportProgress((double)(max-stack.size())/(double)max);
			//check abort
			if(abortRequested())
				return ReachabilityResult(ReachabilityResult::Unknown,
										"Search was aborted.");
		}

		//Take first step of the stack
		State* s = stack.back().state;
		State* ns = State::createState(net);
		bool foundSomething = false;
		for(unsigned int t = stack.back().t; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s->marking(), s->valuation(), ns->marking(), ns->valuation())){
				if(states.add(ns)){
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
		if(!foundSomething)
			stack.pop_back();
	}
	return ReachabilityResult(ReachabilityResult::NotSatisfied,
							"No state satisfying the query exists.");
}



bool DFRSHash::dfshreachable(const PetriNet &net,
							 StateSet *states,
							 State *currentState,
							 PQL::Condition *query){
	if(timer++ == 20){
		timer = 0;
		doAbort = this->abortRequested();
	}
	if(doAbort)
		return false;

	//Does this state satisfy the query?
	if(query->evaluate(PQL::EvaluationContext(currentState->marking(),
											  currentState->valuation())))
		return true;

	for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
		State* child = State::createState(net.numberOfPlaces(), net.numberOfVariables());
		if(net.fire(t, currentState->marking(), currentState->valuation(),
		   child->marking(), child->valuation())){
			//Investigate branch
			if(states->add(child)){
				if(dfshreachable(net, states, child, query)){
					return true;
				}
			} else {
				//State::deleteState(child);
			}
		} else {
			//State::deleteState(child);
		}
	}

	return false;
}
/*
ReachabilityResult DFRSHash::reachable(const PetriNet &net,
									   const MarkVal *initialMarking,
									   const VarVal *initialValuation,
									   PQL::Condition *query){
	//Do we initially satisfy query?
	if(query->evaluate(PQL::EvaluationContext(initialMarking, initialValuation)))
		return ReachabilityResult(ReachabilityResult::Satisfied,
								  "A state satisfying the query was found");
	this->doAbort = false;
	//Create root node
	State* root = State::createState(net.numberOfPlaces(),
									 net.numberOfVariables());
	//Create StateSet
	StateSet* states = new StateSet(net);
	states->add(root);

	//Iterate through branches
	for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
		//Create branch node
		State* branch = State::createState(net.numberOfPlaces(),
										   net.numberOfVariables());		
		//Attempt to fire transition
		if(net.fire(t, initialMarking, initialValuation,
		   branch->marking(), branch->valuation())){
			//Investigate branch
			if(states->add(branch)){
				if(dfshreachable(net, states, branch, query)){
					return ReachabilityResult(ReachabilityResult::Satisfied,
								  "A state satisfying the query was found.");
				}
				if(doAbort)
					return ReachabilityResult(ReachabilityResult::Unknown,
							"Reachability search was aborted.");
			}
			else {}//Kill the unused branch
				//State::deleteState(branch);
		} else {}//Kill the unfirable branch
			//State::deleteState(branch);
		this->reportProgress((double)t/(double)net.numberOfTransitions());
	}



	//No path was found. Unsatisfiable
	return ReachabilityResult(ReachabilityResult::NotSatisfied,
							"No state satisfying the query exists.");
}*/

}}
