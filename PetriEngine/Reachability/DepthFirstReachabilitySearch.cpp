#include "DepthFirstReachabilitySearch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace PetriEngine::PQL;

namespace PetriEngine{ namespace Reachability {

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

	const CoverabilityTreeNode* DepthFirstReachabilitySearch::coverabilityTree(){
		return &_coverabilityTree;
	}

	/** Checks for reachability with DFS */
	bool DepthFirstReachabilitySearch::reachable(const PetriNet &net,
				   const MarkVal* initialMarking,
				   const VarVal* initialAssignment,
				   PQL::Condition* query){

		// Root node
		MarkVal* m0 = new MarkVal[net.numberOfPlaces()];
		VarVal* a0 = new VarVal[net.numberOfVariables()];
		memcpy(m0, initialMarking, net.numberOfPlaces()*sizeof(MarkVal));
		memcpy(a0, initialAssignment, net.numberOfVariables()*sizeof(VarVal));

		this->_coverabilityTree = CoverabilityTreeNode(m0, a0);
		return reachabilityDFS(&_coverabilityTree, net, initialMarking, initialAssignment, query);
	}

} // Reachability
} // PetriEngine
