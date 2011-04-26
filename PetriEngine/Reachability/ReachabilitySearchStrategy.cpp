#include "ReachabilitySearchStrategy.h"

//#include "KarpMillerL1SearchStrategy.h"
#include "DepthFirstReachabilitySearch.h"
#include "BreadthFirstReachabilitySearch.h"
#include "HashUnderApproximation.h"
#include "ClosestFirstReachability.h"
#include "RandomPrioritizedReachability.h"
#include "RandomDFS.h"
#include "HeuristicDFS.h"
#include "../PQL/Contexts.h"
#include <stdio.h>

#define NAME_DFS								"Naive DFS with Hash"
#define NAME_RandomDFS							"Random DFS with Hash"
#define NAME_BFS								"Naive BFS with Hash"
#define NAME_HashUnderApproximation				"Hash under-approximation"
#define NAME_ClosestFirstReachabilityAvg		"Closest-First (Avg)"
#define NAME_ClosestFirstReachabilityExt		"Closest-First (Extreme)"
#define NAME_ClosestFirstReachabilitySum		"Closest-First (Extreme, ArcCount)"
#define NAME_ClosestFirstReachabilityTokenCost	"Closest-First (Extreme, TokenCost)"
#define NAME_TokenCostDeep						"Token Cost Deep"
#define NAME_RandomPrioritizedReachability		"Random-First"
#define NAME_HeuristicDFSTokenCost				"Heuristic DFS (TokenCost)"
#define NAME_ArcCountSum						"ArcCount-Sum"
#define NAME_ExtOrSumAnd						"Extreme-Or Sum-And"
#define NAME_ExtOrSumAndDeep					"Extreme-Or Sum-And Deep!"
#define NAME_HeuristicDFSSumAndExtremeOr		"Heuristic DFS (Extreme-Or Sum-And)"



#include <stdio.h>

namespace PetriEngine { namespace Reachability {

/** List all reachability strategies, return unqiue display names */
std::vector<std::string> ReachabilitySearchStrategy::listStrategies(){
	std::string strats[] = {
		NAME_DFS,
		NAME_RandomDFS,
		NAME_BFS,
		NAME_HashUnderApproximation,
		NAME_ClosestFirstReachabilityAvg,
		NAME_ClosestFirstReachabilityExt,
		NAME_ClosestFirstReachabilitySum,
		NAME_ArcCountSum,
		NAME_ExtOrSumAnd,
		NAME_ExtOrSumAndDeep,
		NAME_ClosestFirstReachabilityTokenCost,
		NAME_TokenCostDeep,
		NAME_RandomPrioritizedReachability,
		NAME_HeuristicDFSSumAndExtremeOr,
		NAME_HeuristicDFSTokenCost
	};
	return std::vector<std::string>(strats, strats + sizeof(strats) / sizeof(std::string));
}

/** Create a reachability strategy from string, NULL if not found */
ReachabilitySearchStrategy* ReachabilitySearchStrategy::createStrategy(const std::string& strategy){
	if(strategy == NAME_DFS)
		return new DepthFirstReachabilitySearch();
	if(strategy == NAME_RandomDFS)
		return new RandomDFS();
	if(strategy == NAME_BFS)
		return new BreadthFirstReachabilitySearch();
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
	if(strategy == NAME_ExtOrSumAnd){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new ClosestFirstReachability((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_ExtOrSumAndDeep){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new ClosestFirstReachability((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_ClosestFirstReachabilitySum){
		int flags = PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme | PQL::DistanceContext::ArcCount;
		return new ClosestFirstReachability((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_ArcCountSum){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme | PQL::DistanceContext::ArcCount;
		return new ClosestFirstReachability((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_ClosestFirstReachabilityTokenCost){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme | PQL::DistanceContext::TokenCost;
		return new ClosestFirstReachability((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_TokenCostDeep){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme | PQL::DistanceContext::TokenCost;
		return new ClosestFirstReachability((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_RandomPrioritizedReachability)
		return new RandomPrioritizedReachability();
	if(strategy == NAME_HeuristicDFSTokenCost){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme | PQL::DistanceContext::TokenCost;
		return new HeuristicDFS((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_HeuristicDFSSumAndExtremeOr){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new HeuristicDFS((PQL::DistanceContext::DistanceStrategy)flags);
	}
	//If we didn't find it
	fprintf(stderr, "Reachability strategy: \"%s\" not found!\n", strategy.c_str());
	return new DepthFirstReachabilitySearch();
}

} // Reachability
} // PetriEngine

