#include "ReachabilitySearchStrategy.h"

//#include "KarpMillerL1SearchStrategy.h"
#include "DepthFirstReachabilitySearch.h"
#include "DFRSHash.h"
#include "HashUnderApproximation.h"
#include "ClosestFirstReachability.h"

#define NAME_DFS								"Naive Karp-Miller DFS"
#define NAME_DFS_Hash							"Naive Karp-Millar DFS with hashing"
#define NAME_HashUnderApproximation				"Hash under-approximation"
#define NAME_ClosestFirstReachabilityAvg		"Closest-First (Avg)"
#define NAME_ClosestFirstReachabilityExt		"Closest-First (Extreme)"

#include <stdio.h>

namespace PetriEngine { namespace Reachability {

/** List all reachability strategies, return unqiue display names */
std::vector<std::string> ReachabilitySearchStrategy::listStrategies(){
	std::vector<std::string> strategies;
	strategies.push_back(NAME_DFS);
	strategies.push_back(NAME_DFS_Hash);
	strategies.push_back(NAME_HashUnderApproximation);
	strategies.push_back(NAME_ClosestFirstReachabilityAvg);
	strategies.push_back(NAME_ClosestFirstReachabilityExt);
	return strategies;
}

/** Create a reachability strategy from string, NULL if not found */
ReachabilitySearchStrategy* ReachabilitySearchStrategy::createStrategy(const std::string& strategy){
	if(strategy == NAME_DFS)
		return new DepthFirstReachabilitySearch();
	if(strategy == NAME_DFS_Hash)
		return new DFRSHash();
	if(strategy == NAME_HashUnderApproximation)
		return new HashUnderApproximation();
	if(strategy == NAME_ClosestFirstReachabilityAvg)
		return new ClosestFirstReachability((PQL::Condition::DistanceStrategy)(PQL::Condition::AndAverage | PQL::Condition::OrAverage));
	if(strategy == NAME_ClosestFirstReachabilityExt)
		return new ClosestFirstReachability((PQL::Condition::DistanceStrategy)(PQL::Condition::AndExtreme | PQL::Condition::OrExtreme));
	//If we didn't find it
	fprintf(stderr, "Reachability strategy: \"%s\" not found!", strategy.c_str());
	return NULL;
}

} // Reachability
} // PetriEngine
