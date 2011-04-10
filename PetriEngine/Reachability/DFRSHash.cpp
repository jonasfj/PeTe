#include "DFRSHash.h"

namespace PetriEngine { namespace Reachability {

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
			} else
				State::deleteState(child);
		} else
			State::deleteState(child);
	}

	return false;
}

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
	StateSet states(net);
	states.add(root);
	//Iterate through branches
	for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
		//Create branch node
		State* branch = State::createState(net.numberOfPlaces(),
										   net.numberOfVariables());		
		//Attempt to fire transition
		if(net.fire(t, initialMarking, initialValuation,
		   branch->marking(), branch->valuation())){
			//Investigate branch
			if(states.add(branch)){
				if(dfshreachable(net, &states, branch, query)){
					return ReachabilityResult(ReachabilityResult::Satisfied,
								  "A state satisfying the query was found.");
				}
			}
			else //Kill the unused branch
				State::deleteState(branch);
		} else //Kill the unfirable branch
			State::deleteState(branch);
		this->reportProgress((double)t/(double)net.numberOfTransitions());
	}

	if(doAbort)
		return ReachabilityResult(ReachabilityResult::Unknown,
							"Reachability search was aborted.");

	//No path was found. Unsatisfiable
	return ReachabilityResult(ReachabilityResult::NotSatisfied,
							"No state satisfying the query exists.");
}

}}
