#include "DepthFirstReachabilitySearch.h"
#include <stdio.h>

namespace PetriEngine{


	bool search(CoverabilityTreeNode* coverTree, PetriNet net, Mark* m){

		printf("\t m(p1)=%i \n",m[0]);
		printf("\t m(p2)=%i \n",m[1]);

		Mark* mNew = new Mark[net.nPlaces()];

		bool deadEnd = true;
		// Fire each transition
		for(int t = 0; t< net.nTransitions(); t++){

			if(net.fire(t, m, mNew)){
				printf("Fire t%i \n",t);
				deadEnd = false;

				// Add child
				CoverabilityTreeNode* child = new CoverabilityTreeNode(coverTree, t, mNew);

				// Check if newMarking same as some other marking
				bool old = child->findDuplicate(net);

				child->setOld(old);

				coverTree->add(child);

				if(child->isOld()){
					printf("m is old \n");
					continue;
				} else {
					printf("Search m \n");
					// add to hash map
					return search(child, net, mNew);
				}
			}

		}

		if(deadEnd){
			// No t's enabled, so the node is "dead"
			coverTree->setDeadEnd(true);
		}
		return false;
	}

	/** Checks for reachability with DFS */
	bool DepthFirstReachabilitySearch::reachable(PetriNet net, Mark* initialMarking){

		// Root node
		CoverabilityTreeNode* coverTree = new CoverabilityTreeNode(initialMarking);
		return search(coverTree, net, initialMarking);
	}

} // PetriEngine
