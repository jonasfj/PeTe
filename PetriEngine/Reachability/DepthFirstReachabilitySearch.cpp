#include "DepthFirstReachabilitySearch.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "../PQL/PQLExpressions.h"

using namespace PetriEngine::PQL;

namespace PetriEngine{ namespace Reachability {

/*  This is the old shit
bool reachabilityDFS(CoverabilityTreeNode* tree,
			   const PetriNet &net,
			   const MarkVal* m,
			   const VarVal* a,
			   PQL::Condition* query){
	//TODO: Print debug info about marking

	bool old = tree->findDuplicate(net);
	tree->setOld(old);

	// If the query is satisfied
	if(query->evaluate(EvaluationContext(m, a)))
		return true;

	if(!tree->isOld() && !tree->isDeadEnd()){
		MarkVal* mNew = EMPTY_MARKING(net.numberOfPlaces());
		VarVal* aNew = EMPTY_ASSIGNMENT(net.numberOfTransitions());

		bool deadEnd = true;
		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			if(net.fire(t, m, a, mNew, aNew)){
				printf("Fire t%i \n",t);
				deadEnd = false;

				// Add child
				CoverabilityTreeNode* child = new CoverabilityTreeNode(tree, t, mNew, aNew);
				tree->add(child);

				return reachabilityDFS(child, net, mNew, aNew, query);
			}
		}
		tree->setDeadEnd(deadEnd);
	} else{
		printf("m is old \n");
	}
	return false;
}
*/
/*This is the new shit*/
/** Internal recursive reachability searcher */
bool DepthFirstReachabilitySearch::dfsReachable(State *oldStates,
									const PetriNet &net,
									const MarkVal *m,
									const VarVal *a,
									PQL::Condition *query){
	// Check for duplicate
	if(oldStates->isLoop(net))
		return false;

	// If not duplicate, check if evaluates
	if(query->evaluate(EvaluationContext(m, a)))
		return true;

	// If not duplicate and does not evaluate, fire a transition, and call recursively
	for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
		//std::cout<<"Transition: "<<t<<std::endl;
		//New state for firing
		State* child = State::createState(net.numberOfPlaces(), net.numberOfVariables(), oldStates);

		if(net.fire(t, m, a, child->marking(), child->valuation())){
			//Explore this new path
			if(dfsReachable(child, net, child->marking(), child->valuation(), query)) {
				delete child;
				child = NULL;
				return true;
			}
		}
		delete child;
		child = NULL;
	}

	// If no result has been found, return false
	return false;
}

/** Checks for reachability with DFS */
ReachabilityResult DepthFirstReachabilitySearch::reachable(const PetriNet &net,
									const MarkVal* initialMarking,
									const VarVal* initialAssignment,
									PQL::Condition* query){
	// Create copies of the marking and variable assignment
	MarkVal* m0 = new MarkVal[net.numberOfPlaces()];
	memcpy(m0, initialMarking, net.numberOfPlaces()*sizeof(MarkVal));
	VarVal* a0 = new VarVal[net.numberOfVariables()];
	memcpy(a0, initialAssignment, net.numberOfVariables()*sizeof(VarVal));

	// Create root node of the state space
	State* root = State::createState(net.numberOfPlaces(), net.numberOfVariables());

	// Recursively handle reachability check
	bool result = dfsReachable(root,
					 net,
					 initialMarking,
					 initialAssignment,
					 query);
	delete root;
	root = NULL;
	if(result)
		return ReachabilityResult(ReachabilityResult::Satisfied,
								  "A state satisfying the queue was found");
	return ReachabilityResult(ReachabilityResult::NotSatisfied,
							  "No state satisfying the queue exists");
}

/** Set the progress reporter */
void DepthFirstReachabilitySearch::setProgressReporter(ProgressReporter *reporter){
	_reporter = reporter;
}

} // Reachability
} // PetriEngine
