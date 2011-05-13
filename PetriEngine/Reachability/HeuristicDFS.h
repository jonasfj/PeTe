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
#ifndef HEURISTICDFS_H
#define HEURISTICDFS_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/DistanceMatrix.h"
#include "../PQL/Contexts.h"

namespace PetriEngine {
namespace Reachability {

/** Heuristic Depth First Reachability Search */
class HeuristicDFS : public ReachabilitySearchStrategy
{
public:
	/** Create instance of HeuristicDFS
	 * @param distanceStrategy Distance strategy, for computation of distance to query
	 */
	HeuristicDFS(PQL::DistanceContext::DistanceStrategy distanceStrategy){
		_distanceStrategy = distanceStrategy;
	}
	/** Perform reachability check using random DFS with a hash table */
	ReachabilityResult reachable(const PetriNet& net,
								 const MarkVal* m0,
								 const VarVal* v0,
								 PQL::Condition* query);
private:
	PQL::DistanceContext::DistanceStrategy _distanceStrategy;
};

} // Reachability
} // PetriEngine

#endif // HEURISTICDFS_H
