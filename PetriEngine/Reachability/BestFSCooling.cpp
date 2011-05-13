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
#include "BestFSCooling.h"

#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/PriorityQueue.h"
#include "../Structures/EnhancedPriorityQueue.h"
#include "../Structures/StateSet.h"
#include "../Structures/StateAllocator.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

using namespace PetriEngine::Structures;
using namespace PetriEngine::PQL;

namespace PetriEngine{
namespace Reachability{

ReachabilityResult BestFSCooling::reachable(const PetriNet &net,
											const MarkVal *m0,
											const VarVal *v0,
											PQL::Condition *query){
	StateAllocator<> allocator(net);

	State* s0 = allocator.createState();
	memcpy(s0->marking(), m0, sizeof(MarkVal) * net.numberOfPlaces());
	memcpy(s0->valuation(), v0, sizeof(VarVal) * net.numberOfVariables());

	if(query->evaluate(*s0))
		return ReachabilityResult(ReachabilityResult::Satisfied, "Satisfied initially", 0, 0);

	//Initialize subclasses
	initialize(query, net);

	StateSet states(net);
	_states = &states;
	states.add(s0);
	//PriorityQueue<State*> queue;
	EnhancedPriorityQueue<State*> queue;
	queue.push(0, s0);

	//Allocate new state
	State* ns = allocator.createState();
	State* ns2 = allocator.createState();

	int count = 0;
	BigInt expandedStates = 0;
	BigInt exploredStates = 0;
	size_t max = 1;
	while(!queue.empty()){
		if(count++ & 1<<17){
			count = 0;
			if(queue.size() > max)
				max = queue.size();
			this->reportProgress((double)(max - queue.size()) / ((double)(max)));
			if(this->abortRequested())
				return ReachabilityResult(ReachabilityResult::Unknown,
										  "Query aborted!");
		}

		//Take something out of the queue
		double topP = queue.topPriority();
		State* s = queue.pop(depthFirst);
		expandedStates++;

		int heatFactor = (int)floor(topP / net.numberOfPlaces());

		// Attempt to fire each transition
		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s->marking(), s->valuation(), ns->marking(), ns->valuation())){
				//If it's new
				if(states.add(ns)){
					exploredStates++;
					//Set parent and transition for the state
					ns->setParent(s);
					ns->setTransition(t);

					//Test query
					if(query->evaluate(*ns))
						return ReachabilityResult(ReachabilityResult::Satisfied,
												  "Query was satified!", expandedStates, exploredStates, ns->pathLength());

					// Insert in queue, with given priority
					double bestp = priority(ns, query, net);
					queue.push(bestp, ns);

					//Do the cool step
					if(heatFactor > 1 && net.fire(t, s, ns2, heatFactor)){
						if(states.add(ns2)){
							queue.push(priority(ns2, query, net), ns2);
							ns2 = allocator.createState();
						}
					}


					//Allocate new stake, as states take ownership
					ns = allocator.createState();
				}
			}
		}
	}

	return ReachabilityResult(ReachabilityResult::NotSatisfied,
							  "Query cannot be satisfied!", expandedStates, exploredStates);
}

double BestFSCooling::priority(const Structures::State *state,
							   const PQL::Condition *query,
							   const PetriNet& net){
	PQL::DistanceContext context(net,
								 _distanceStrategy,
								 state->marking(),
								 state->valuation(),
								 _dm);
	double d = query->distance(context);
	if(_lookahead > 0){
		double d2 = lookaheadDistance(net, state, query, _lookahead);
		d = d < d2 ? d : d2;
	}
	return d;
}

void BestFSCooling::initialize(const PQL::Condition*,
							   const PetriNet& net){
	_dm = new Structures::DistanceMatrix(net);
}

double BestFSCooling::lookaheadDistance(const PetriNet& net,
										const Structures::State* state,
										const PQL::Condition* query,
										int depth){
	double min = -1;
	State* ns = (State*)alloca(StateAllocator<>::stateSize(net));
	StateAllocator<>::initializeState(ns, net);
	for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
		if(net.fire(t, state, ns) && !_states->contains(ns)){
			PQL::DistanceContext context(net,
										 _distanceStrategy,
										 ns->marking(),
										 ns->valuation(),
										 _dm);
			double d = query->distance(context);
			if(d < min || min == -1)
				min = d;
			if(depth - 1 > 0)
				d = lookaheadDistance(net, ns, query, depth-1);
			if(d < min || min == -1)
				min = d;
		}
	}
	return min;
}

} // Reachability
} // PetriEngine
