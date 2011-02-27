#include "DepthFirstReachabilitySearch.h"

namespace PetriEngine{

	bool search(CoverabilityTreeNode* coverTree, PetriNet net, Mark* initialMarking){

		Mark* newMarking = new Mark[net.nPlaces()];
		bool deadEnd = true;
		// Fire each transition
		for(int t = 0; t< net.nTransitions(); t++){

			if(net.fire(t, initialMarking, newMarking)){
				deadEnd = false;
				// Add child
				CoverabilityTreeNode* child = new CoverabilityTreeNode(coverTree, t, newMarking);

				// Check if newMarking same as some other marking
				bool old = child->findDuplicate(net);
				child->setOld(old);
				coverTree->add(child);

				if(old){
					continue;
				} else {
					// add to hash map
					return search(child, net, newMarking);
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
	bool reachable(PetriNet net, Mark* initialMarking){

		// Root node
		CoverabilityTreeNode* coverTree = new CoverabilityTreeNode(initialMarking);
		return search(coverTree, net, initialMarking);
	}

} // PetriEngine
