#include "ReachabilitySearchStrategy.h"

//#include "KarpMillerL1SearchStrategy.h"
#include "DepthFirstReachabilitySearch.h"
#include "DFRSHash.h"

#include <stdio.h>

namespace PetriEngine { namespace Reachability {

/** List all reachability strategies, return unqiue display names */
std::vector<std::string> ReachabilitySearchStrategy::listStrategies(){
	std::vector<std::string> strategies;
	//strategies.push_back("Karp-Miller L1");
	strategies.push_back("Naive Karp-Miller DFS");
	strategies.push_back("Naive Karp-Millar DFS with hashing");
	return strategies;
}

/** Create a reachability strategy from string, NULL if not found */
ReachabilitySearchStrategy* ReachabilitySearchStrategy::createStrategy(const std::string& strategy){
	//if(strategy == "Karp-Miller L1")
	//	return new KarpMillerL1SearchStrategy();
	if(strategy == "Naive Karp-Miller DFS")
		return new DepthFirstReachabilitySearch();
	if(strategy == "Naive Karp-Millar DFS with hashing")
		return new DFRSHash();
	//If we didn't find it
	fprintf(stderr, "Reachability strategy: \"%s\" not found!", strategy.c_str());
	return NULL;
}

} // Reachability
} // PetriEngine
