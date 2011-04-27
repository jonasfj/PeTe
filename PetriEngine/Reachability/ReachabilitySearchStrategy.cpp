#include "ReachabilitySearchStrategy.h"

//#include "KarpMillerL1SearchStrategy.h"
#include "DepthFirstReachabilitySearch.h"
#include "BreadthFirstReachabilitySearch.h"
#include "HashUnderApproximation.h"
#include "BestFirstReachabilitySearch.h"
#include "RandomDFS.h"
#include "HeuristicDFS.h"
#include "../PQL/Contexts.h"
#include <stdio.h>

//Basic
#define NAME_DFS								"Naive DFS with Hash"
#define NAME_RandomDFS							"Random DFS with Hash"
#define NAME_BFS								"Naive BFS with Hash"
//Heuristics
#define NAME_DFSArcCount						"DFS-ArcCount"
#define NAME_DFSTokenCost						"DFS-TokenCost"
//BestFirsts-ArcCount
#define NAME_BestFSArcCount2Extreme				"BestFS-ArcCount (Extreme, Extreme)"
#define NAME_BestFSArcCountExtremeAverage		"BestFS-ArcCount (Extreme, Average)"
#define NAME_BestFSArcCountAverageExtreme		"BestFS-ArcCount (Average, Extreme)"
#define NAME_BestFSArcCount2Average				"BestFS-ArcCount (Average, Average)"
#define NAME_BestFSArcCountSumExtreme			"BestFS-ArcCount (Sum, Extreme)"
#define NAME_BestFSArcCountSumAverage			"BestFS-ArcCount (Sum, Average)"

#define NAME_BestFSArcCountDeep2Extreme			"BestFS-ArcCount-Deep (Extreme, Extreme)"
#define NAME_BestFSArcCountDeepExtremeAverage	"BestFS-ArcCount-Deep (Extreme, Average)"
#define NAME_BestFSArcCountDeepAverageExtreme	"BestFS-ArcCount-Deep (Average, Extreme)"
#define NAME_BestFSArcCountDeep2Average			"BestFS-ArcCount-Deep (Average, Average)"
#define NAME_BestFSArcCountDeepSumExtreme		"BestFS-ArcCount-Deep (Sum, Extreme)"
#define NAME_BestFSArcCountDeepSumAverage		"BestFS-ArcCount-Deep (Sum, Average)"

//BestFirsts-TokenCost
#define NAME_BestFSTokenCost2Extreme			"BestFS-TokenCost (Extreme, Extreme)"
#define NAME_BestFSTokenCostExtremeAverage		"BestFS-TokenCost (Extreme, Average)"
#define NAME_BestFSTokenCostAverageExtreme		"BestFS-TokenCost (Average, Extreme)"
#define NAME_BestFSTokenCost2Average			"BestFS-TokenCost (Average, Average)"
#define NAME_BestFSTokenCostSumExtreme			"BestFS-TokenCost (Sum, Extreme)"
#define NAME_BestFSTokenCostSumAverage			"BestFS-TokenCost (Sum, Average)"

#define NAME_BestFSTokenCostDeep2Extreme		"BestFS-TokenCost-Deep (Extreme, Extreme)"
#define NAME_BestFSTokenCostDeepExtremeAverage	"BestFS-TokenCost-Deep (Extreme, Average)"
#define NAME_BestFSTokenCostDeepAverageExtreme	"BestFS-TokenCost-Deep (Average, Extreme)"
#define NAME_BestFSTokenCostDeep2Average		"BestFS-TokenCost-Deep (Average, Average)"
#define NAME_BestFSTokenCostDeepSumExtreme		"BestFS-TokenCost-Deep (Sum, Extreme)"
#define NAME_BestFSTokenCostDeepSumAverage		"BestFS-TokenCost-Deep (Sum, Average)"

//Below defines are candidates for deletion
#define NAME_HashUnderApproximation				"Hash under-approximation"
#define NAME_ClosestFirstReachabilityAvg		"Closest-First (Avg)"
#define NAME_ClosestFirstReachabilityExt		"Closest-First (Extreme)"
#define NAME_ClosestFirstReachabilitySum		"Closest-First (Extreme, ArcCount)"
#define NAME_ClosestFirstReachabilityTokenCost	"Closest-First (Extreme, TokenCost)"
#define NAME_TokenCostDeep						"Token Cost Deep"
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
		NAME_DFSArcCount,
		NAME_DFSTokenCost,
		NAME_BestFSArcCount2Extreme,
		NAME_BestFSArcCountExtremeAverage,
		NAME_BestFSArcCountAverageExtreme,
		NAME_BestFSArcCount2Average,
		NAME_BestFSArcCountSumExtreme,
		NAME_BestFSArcCountSumAverage,
		NAME_BestFSArcCountDeep2Extreme,
		NAME_BestFSArcCountDeepExtremeAverage,
		NAME_BestFSArcCountDeepAverageExtreme,
		NAME_BestFSArcCountDeep2Average,
		NAME_BestFSArcCountDeepSumExtreme,
		NAME_BestFSArcCountDeepSumAverage,
		NAME_BestFSTokenCost2Extreme,
		NAME_BestFSTokenCostExtremeAverage,
		NAME_BestFSTokenCostAverageExtreme,
		NAME_BestFSTokenCost2Average,
		NAME_BestFSTokenCostSumExtreme,
		NAME_BestFSTokenCostSumAverage,
		NAME_BestFSTokenCostDeep2Extreme,
		NAME_BestFSTokenCostDeepExtremeAverage,
		NAME_BestFSTokenCostDeepAverageExtreme,
		NAME_BestFSTokenCostDeep2Average,
		NAME_BestFSTokenCostDeepSumExtreme,
		NAME_BestFSTokenCostDeepSumAverage
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
	if(strategy == NAME_DFSArcCount){
		int flags = PQL::DistanceContext::ArcCount;
		return new HeuristicDFS((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_DFSTokenCost){
		int flags = PQL::DistanceContext::TokenCost;
		return new HeuristicDFS((PQL::DistanceContext::DistanceStrategy)flags);
	}

	//BestFSArcCount
	if(strategy == NAME_BestFSArcCount2Extreme){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSArcCountExtremeAverage){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSArcCount2Extreme){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSArcCountAverageExtreme){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSArcCount2Average){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSArcCountSumExtreme){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSArcCountSumAverage){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndSum | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}

	//BestFSArcCount-Deep
	if(strategy == NAME_BestFSArcCountDeep2Extreme){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSArcCountDeepExtremeAverage){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSArcCountDeep2Extreme){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSArcCountDeepAverageExtreme){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSArcCountDeep2Average){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSArcCountDeepSumExtreme){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSArcCountDeepSumAverage){
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndSum | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}

	//BestFSTokenCost
	if(strategy == NAME_BestFSTokenCost2Extreme){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSTokenCostExtremeAverage){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSTokenCost2Average){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSTokenCostAverageExtreme){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSTokenCostSumExtreme){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSTokenCostSumAverage){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndSum | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}

	//BestFSTokenCost-Deep
	if(strategy == NAME_BestFSTokenCostDeep2Extreme){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSTokenCostDeepExtremeAverage){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSTokenCostDeep2Average){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSTokenCostDeepAverageExtreme){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSTokenCostDeepSumExtreme){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSTokenCostDeepSumAverage){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndSum | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}

	//If we didn't find it
	fprintf(stderr, "Reachability strategy: \"%s\" not found!\n", strategy.c_str());
	return new DepthFirstReachabilitySearch();
}

} // Reachability
} // PetriEngine

