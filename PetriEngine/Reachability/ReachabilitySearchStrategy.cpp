#include "ReachabilitySearchStrategy.h"

//#include "KarpMillerL1SearchStrategy.h"
#include "DepthFirstReachabilitySearch.h"
#include "DFRSHash.h"
#include "HashUnderApproximation.h"
#include "ClosestFirstReachability.h"
#include "../PQL/Contexts.h"

#define NAME_DFS								"Naive Karp-Miller DFS"
#define NAME_DFS_Hash							"Naive Karp-Millar DFS with hashing"
#define NAME_HashUnderApproximation				"Hash under-approximation"
#define NAME_ClosestFirstReachabilityAvg		"Closest-First (Avg)"
#define NAME_ClosestFirstReachabilityExt		"Closest-First (Extreme)"
#define NAME_ClosestFirstReachabilitySum		"Closest-First (Extreme, ArcCount)"
#define NAME_ClosestFirstReachabilityTokenCost	"Closest-First (Extreme, TokenCost)"



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
	strategies.push_back(NAME_ClosestFirstReachabilitySum);
	strategies.push_back(NAME_ClosestFirstReachabilityTokenCost);
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
	if(strategy == NAME_ClosestFirstReachabilityAvg){
		int flags = PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrAverage;
		return new ClosestFirstReachability((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_ClosestFirstReachabilityExt){
		int flags = PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new ClosestFirstReachability((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_ClosestFirstReachabilitySum){
		int flags = PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme | PQL::DistanceContext::ArcCount;
		return new ClosestFirstReachability((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_ClosestFirstReachabilityTokenCost){
		int flags = PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme | PQL::DistanceContext::TokenCost;
		return new ClosestFirstReachability((PQL::DistanceContext::DistanceStrategy)flags);
	}
	//If we didn't find it
	fprintf(stderr, "Reachability strategy: \"%s\" not found!", strategy.c_str());
	return NULL;
}

} // Reachability
} // PetriEngine

