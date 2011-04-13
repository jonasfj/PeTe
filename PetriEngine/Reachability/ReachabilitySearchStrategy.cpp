#include "ReachabilitySearchStrategy.h"

//#include "KarpMillerL1SearchStrategy.h"
#include "DepthFirstReachabilitySearch.h"
#include "DFRSHash.h"
#include "HashUnderApproximation.h"

#define NAME_DFS "Naive Karp-Miller DFS"
#define NAME_DFS_Hash "Naive Karp-Millar DFS with hashing"
#define NAME_HashUnderApproximation "Hash under-approximation"

#include <stdio.h>

namespace PetriEngine { namespace Reachability {

/** List all reachability strategies, return unqiue display names */
std::vector<std::string> ReachabilitySearchStrategy::listStrategies(){
	std::vector<std::string> strategies;
	//strategies.push_back("Karp-Miller L1");
	strategies.push_back(NAME_DFS);
	strategies.push_back(NAME_DFS_Hash);
	strategies.push_back(NAME_HashUnderApproximation);
	return strategies;
}

/** Create a reachability strategy from string, NULL if not found */
ReachabilitySearchStrategy* ReachabilitySearchStrategy::createStrategy(const std::string& strategy){
	//if(strategy == "Karp-Miller L1")
	//	return new KarpMillerL1SearchStrategy();
	if(strategy == NAME_DFS)
		return new DepthFirstReachabilitySearch();
	if(strategy == NAME_DFS_Hash)
		return new DFRSHash();
	if(strategy == NAME_HashUnderApproximation)
		return new HashUnderApproximation();
	//If we didn't find it
	fprintf(stderr, "Reachability strategy: \"%s\" not found!", strategy.c_str());
	return NULL;
}

} // Reachability
} // PetriEngine
