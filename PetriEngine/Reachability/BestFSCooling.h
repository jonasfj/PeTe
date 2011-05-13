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
#ifndef BESTFSCOOLING_H
#define BESTFSCOOLING_H

#include "ReachabilitySearchStrategy.h"
#include "../Structures/State.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/DistanceMatrix.h"


namespace PetriEngine{

namespace Structures{
	class StateSet;
}

namespace Reachability{


/** Best first reachability strategy */
class BestFSCooling : public ReachabilitySearchStrategy
{
public:
	/** Create instance of BestFirstReachability
	 * @param distanceStrategy Distance strategy, for computation of distance to query
	 */
	BestFSCooling(PQL::DistanceContext::DistanceStrategy distanceStrategy,
								bool depthFirst = true,
								int lookahead = 0){
		this->_distanceStrategy = distanceStrategy;
		this->depthFirst = depthFirst;
		_lookahead = lookahead;
	}
	ReachabilityResult reachable(const PetriNet &net,
								 const MarkVal* m0,
								 const VarVal* v0,
								 PQL::Condition* query);
private:
	void initialize(const PQL::Condition* query, const PetriNet& net);
	/** Method for prioritizing different states, lower priority is better */
	double priority(const Structures::State* state,
					const PQL::Condition* query,
					const PetriNet& net);
	PQL::DistanceContext::DistanceStrategy _distanceStrategy;
	Structures::DistanceMatrix* _dm;
	int _lookahead;
	bool depthFirst;

	/** Find the best distance in depth steps from state */
	double lookaheadDistance(const PetriNet& net,
							 const Structures::State* state,
							 const PQL::Condition* query,
							 int depth);
	Structures::StateSet* _states;
};

} // Reachability
} // PetriEngine

#endif // BESTFSCOOLING_H
