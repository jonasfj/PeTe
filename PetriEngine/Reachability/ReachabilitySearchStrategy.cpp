/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ReachabilitySearchStrategy.h"

//#include "KarpMillerL1SearchStrategy.h"
#include "DepthFirstReachabilitySearch.h"
#include "BreadthFirstReachabilitySearch.h"
#include "HashUnderApproximation.h"
#include "BestFirstReachabilitySearch.h"
#include "LinearOverApprox.h"
#include "RandomDFS.h"
#include "BestFSCooling.h"
#include "UltimateSearch.h"
#include "HeuristicDFS.h"
#include "MagicSearch.h"
#include "../PQL/Contexts.h"
#include <stdio.h>

//Basic
#define NAME_DFS								"Naive DFS with Hash"
#define NAME_RandomDFS							"Random DFS with Hash"
#define NAME_BFS								"Naive BFS with Hash"
//Heuristics
#define NAME_DFSArcCount						"DFS-ArcCount"
#define NAME_DFSTokenCost						"DFS-TokenCost"
#define NAME_DFSDelta							"DFS-Delta"

//BestFirsts-Delta
#define NAME_BestFSDelta2Extreme				"BestFS-Delta (Extreme, Extreme)"
#define NAME_BestFSDeltaExtremeAverage			"BestFS-Delta (Extreme, Average)"
#define NAME_BestFSDeltaAverageExtreme			"BestFS-Delta (Average, Extreme)"
#define NAME_BestFSDelta2Average				"BestFS-Delta (Average, Average)"
#define NAME_BestFSDeltaSumExtreme				"BestFS-Delta (Sum, Extreme)"
#define NAME_BestFSDeltaSumAverage				"BestFS-Delta (Sum, Average)"

#define NAME_BestFSDeltaSumExtremeBFS			"BestFS-Delta (Sum, Extreme) BFS"
#define NAME_BestFSDeltaSumExtremeDFS			"BestFS-Delta (Sum, Extreme) DFS"

#define NAME_BestFSDeltaDeep2Extreme			"BestFS-Delta-Deep (Extreme, Extreme)"
#define NAME_BestFSDeltaDeepExtremeAverage		"BestFS-Delta-Deep (Extreme, Average)"
#define NAME_BestFSDeltaDeepAverageExtreme		"BestFS-Delta-Deep (Average, Extreme)"
#define NAME_BestFSDeltaDeep2Average			"BestFS-Delta-Deep (Average, Average)"
#define NAME_BestFSDeltaDeepSumExtreme			"BestFS-Delta-Deep (Sum, Extreme)"
#define NAME_BestFSDeltaDeepSumAverage			"BestFS-Delta-Deep (Sum, Average)"

// Experiments (Don't need to play with Avg/Extreme/Sum setting here:
#define NAME_BestFSDeltaSumExtremeLH1			"BestFS-Delta (Sum, Extreme) Lookahead 1"
#define NAME_BestFSDeltaSumExtremeLH2			"BestFS-Delta (Sum, Extreme) Lookahead 2"
#define NAME_BestFSDeltaSumExtremeLH3			"BestFS-Delta (Sum, Extreme) Lookahead 3"
#define NAME_BestFSDeltaSumExtremeLH4			"BestFS-Delta (Sum, Extreme) Lookahead 4"
#define NAME_BestFSDeltaSumExtremeLH5			"BestFS-Delta (Sum, Extreme) Lookahead 5"

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

//Over-approximation by linear programming
#define NAME_LinearOverApprox					"Linear over-approximation"

//Cooling BestFS
#define NAME_BestFSCoolingDeltaDFS				"BestFS-Cooling-Delta (DFS)"

//Adaptive Memory Search
#define NAME_MagicSearchBFS						"Adaptive memory BFS Delta"
#define NAME_MagicSearchDFS10						"Adaptive memory DFS Delta 10"
#define NAME_MagicSearchDFS50						"Adaptive memory DFS Delta 50"
#define NAME_MagicSearchDFS100						"Adaptive memory DFS Delta 100"
#define NAME_MagicSearchDFS200						"Adaptive memory DFS Delta 200"

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


#define NAME_UltimateSearch						"BestFS Ultimate Edition"

#include <stdio.h>

using namespace PetriEngine::Structures;

namespace PetriEngine { namespace Reachability {

/** List all reachability strategies, return unqiue display names */
std::vector<std::string> ReachabilitySearchStrategy::listStrategies(){
	std::string strats[] = {
		NAME_DFS,
		NAME_RandomDFS,
		NAME_BFS,
		NAME_DFSArcCount,
		NAME_DFSTokenCost,
		NAME_DFSDelta,
		NAME_BestFSDeltaSumExtreme,
		NAME_BestFSDeltaDeepSumExtreme,
		/*NAME_BestFSDeltaDeepSumAverage
		NAME_BestFSDeltaSumAverage,
		NAME_BestFSDeltaDeep2Extreme,
		NAME_BestFSDeltaDeepExtremeAverage,
		NAME_BestFSDeltaDeepAverageExtreme,
		NAME_BestFSDeltaDeep2Average,
		NAME_BestFSDelta2Extreme,
		NAME_BestFSDeltaExtremeAverage,
		NAME_BestFSDeltaAverageExtreme,
		NAME_BestFSDelta2Average,
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
		NAME_BestFSTokenCostDeepSumAverage*/
		NAME_BestFSDeltaSumExtremeBFS,
		NAME_BestFSDeltaSumExtremeDFS,
		NAME_LinearOverApprox,
		NAME_BestFSDeltaSumExtremeLH1,
		NAME_BestFSDeltaSumExtremeLH2,
		NAME_BestFSDeltaSumExtremeLH3,
		NAME_BestFSDeltaSumExtremeLH4,
		NAME_BestFSDeltaSumExtremeLH5,
		NAME_BestFSCoolingDeltaDFS,
		NAME_MagicSearchBFS,
		NAME_UltimateSearch
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
		int flags = PQL::DistanceContext::ArcCount | PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new HeuristicDFS((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_DFSTokenCost){
		int flags = PQL::DistanceContext::TokenCost | PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;;
		return new HeuristicDFS((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_DFSDelta){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;;
		return new HeuristicDFS((PQL::DistanceContext::DistanceStrategy)flags);
	}

	//BestFSDelta
	/*if(strategy == NAME_BestFSDeltaAverageExtreme){
		int flags = PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSDelta2Average){
		int flags = PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSDelta2Extreme){
		int flags = PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSDeltaExtremeAverage){
		int flags = PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSDelta2Extreme){
		int flags = PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}
	if(strategy == NAME_BestFSDeltaSumAverage){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}*/
	if(strategy == NAME_BestFSDeltaSumExtreme){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags);
	}

	//BestFSDelta-Deep
	/*if(strategy == NAME_BestFSDeltaDeepAverageExtreme){
		int flags = PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSDeltaDeep2Average){
		int flags = PQL::DistanceContext::AndAverage | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSDeltaDeep2Extreme){
		int flags = PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSDeltaDeepExtremeAverage){
		int flags = PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSDeltaDeep2Extreme){
		int flags = PQL::DistanceContext::AndExtreme | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}
	if(strategy == NAME_BestFSDeltaDeepSumAverage){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrAverage;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}*/
	if(strategy == NAME_BestFSDeltaDeepSumExtreme){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, true);
	}

	//BestFSTokenCost
	/*if(strategy == NAME_BestFSTokenCost2Extreme){
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
	}*/

	//BestFSDelta-LHx
	if(strategy == NAME_BestFSDeltaSumExtremeLH1){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, false, true, 1);
	}
	if(strategy == NAME_BestFSDeltaSumExtremeLH2){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, false, true, 2);
	}
	if(strategy == NAME_BestFSDeltaSumExtremeLH3){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, false, true, 3);
	}
	if(strategy == NAME_BestFSDeltaSumExtremeLH4){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, false, true, 4);
	}
	if(strategy == NAME_BestFSDeltaSumExtremeLH5){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, false, true, 5);
	}


	if(strategy == NAME_BestFSDeltaSumExtremeBFS){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, false, false);
	}
	if(strategy == NAME_BestFSDeltaSumExtremeDFS){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFirstReachabilitySearch((PQL::DistanceContext::DistanceStrategy)flags, false, true);
	}


	//Linear over-approximation
	if(strategy == NAME_LinearOverApprox){
		return new LinearOverApprox();
	}

	if(strategy == NAME_BestFSCoolingDeltaDFS){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;
		return new BestFSCooling((PQL::DistanceContext::DistanceStrategy)flags, true);
	}

	if(strategy == NAME_MagicSearchBFS){
		int flags = PQL::DistanceContext::AndSum | PQL::DistanceContext::OrExtreme;;
		return new MagicSearch((PQL::DistanceContext::DistanceStrategy)flags,
								false,
								Scale(0, 0.5, 0),
								Scale(0.01, 0, 0));
	}

	if(strategy == NAME_UltimateSearch){
		return new UltimateSearch();
	}

	//If we didn't find it
	fprintf(stderr, "Reachability strategy: \"%s\" not found!\n", strategy.c_str());
	return new DepthFirstReachabilitySearch();
}

} // Reachability
} // PetriEngine

