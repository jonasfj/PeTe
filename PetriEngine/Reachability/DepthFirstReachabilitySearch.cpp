#include "DepthFirstReachabilitySearch.h"
#include <stdio.h>
#include <stdlib.h>

using namespace PetriEngine::PQL;

namespace PetriEngine{ namespace Reachability {

	bool reachabilityDFS(CoverabilityTreeNode* coverTree, PetriNet net, MarkVal* m, VarVal* a, Condition* query){

		//TODO: Print debug info about marking

		bool old = coverTree->findDuplicate(net);
		coverTree->setOld(old);

		// If the query is satisfied
		if(query->evaluate(EvaluationContext(m, a)))
			return true;

		if(!coverTree->isOld() && !coverTree->isDeadEnd()){
			MarkVal* mNew = EMPTY_MARKING(net.numberOfPlaces());
			VarVal* aNew = EMPTY_ASSIGNMENT(net.numberOfTransitions());

			bool deadEnd = true;
			for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
				if(net.fire(t, m, a, mNew, aNew)){
					printf("Fire t%i \n",t);
					deadEnd = false;

					// Add child
					CoverabilityTreeNode* child = new CoverabilityTreeNode(coverTree, t, mNew, aNew);
					coverTree->add(child);

					return reachabilityDFS(child, net, mNew, aNew, query);
				}
			}
			coverTree->setDeadEnd(deadEnd);
		} else{
			printf("m is old \n");
		}
		return false;
	}

	const CoverabilityTreeNode* DepthFirstReachabilitySearch::coverabilityTree(){
		return this->_coverabilityTree;
	}

	/** Checks for reachability with DFS */
	bool DepthFirstReachabilitySearch::reachable(PetriNet net, MarkVal* initialMarking, VarVal* initialAssignment, Condition* query){

		// Root node
		CoverabilityTreeNode* coverTree = new CoverabilityTreeNode(initialMarking, initialAssignment);
		this->_coverabilityTree = coverTree;
		return reachabilityDFS(coverTree, net, initialMarking, initialAssignment, query);
	}

} // Reachability
} // PetriEngine
