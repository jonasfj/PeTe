#include "DepthFirstReachabilitySearch.h"
#include <stdio.h>

namespace PetriEngine{

	bool reachabilityDFS(CoverabilityTreeNode* coverTree, PetriNet net, Mark* m){

		//TODO: Print debug info about marking

		bool old = coverTree->findDuplicate(net);
		coverTree->setOld(old);

		if(!coverTree->isOld() && !coverTree->isDeadEnd()){
			Mark* mNew = net.makeEmptyMarking();

			bool deadEnd = true;
			for(int t = 0; t < net.nTransitions(); t++){
				if(net.fire(t, m, mNew)){
					printf("Fire t%i \n",t);
					deadEnd = false;

					// Add child
					CoverabilityTreeNode* child = new CoverabilityTreeNode(coverTree, t, mNew);
					coverTree->add(child);

					return reachabilityDFS(child, net, mNew);
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
	bool DepthFirstReachabilitySearch::reachable(PetriNet net, Mark* initialMarking){

		// Root node
		CoverabilityTreeNode* coverTree = new CoverabilityTreeNode(initialMarking);
		this->_coverabilityTree = coverTree;
		return reachabilityDFS(coverTree, net, initialMarking);
	}

} // PetriEngine
