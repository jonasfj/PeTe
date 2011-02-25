#include "DepthFirstReachabilitySearch.h"

namespace PetriEngine{

	/** Checks for reachability with DFS */
	bool reachable(PetriNet net, Mark* initialMarking){

		Mark* newMarking;
		for(int t = 0; t< 1000; t++){
			if(net.fire(t, initialMarking, newMarking)){
				// Check if newMarking same as some other marking
				bool old = false;
				if(old){
					// tag newMarking as old and skip to next
					continue;
				}

			} else {
				// tag initialMarking as dead-end
			}
		}

		return false;
	}


} // PetriEngine
