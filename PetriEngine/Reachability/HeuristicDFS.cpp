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
#include "HeuristicDFS.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/StateSet.h"
#include "../Structures/StateAllocator.h"

#include <list>
#include <string.h>
#include <stdlib.h>

using namespace PetriEngine::PQL;
using namespace PetriEngine::Structures;

namespace PetriEngine {
namespace Reachability {


ReachabilityResult HeuristicDFS::reachable(const PetriNet& net,
										   const MarkVal* m0,
										   const VarVal* v0,
										   PQL::Condition* query){
	//Do we initially satisfy query?
	if(query->evaluate(PQL::EvaluationContext(m0, v0)))
		return ReachabilityResult(ReachabilityResult::Satisfied,
								  "A state satisfying the query was found");

	StateSet states(net);
	StateAllocator<> allocator(net);
	std::list<State*> stack;

	State* s0 = allocator.createState();
	memcpy(s0->marking(), m0, sizeof(MarkVal)*net.numberOfPlaces());
	memcpy(s0->valuation(), v0, sizeof(VarVal)*net.numberOfVariables());
	stack.push_back(s0);
	states.add(s0);

	Structures::DistanceMatrix distanceMatrix(net);

	State* ns = allocator.createState();
	unsigned int max = 0;
	int count = 0;
	BigInt expandedStates = 0;
	BigInt exploredStates = 0;
	while(!stack.empty()){
		// Progress reporting and abort checking
		if(count++ & 1<<16){
			if(stack.size() > max)
				max = stack.size();
			count = 0;
			// Report progress
			reportProgress((double)(max - stack.size())/(double)max);
			// Check abort
			if(abortRequested())
				return ReachabilityResult(ReachabilityResult::Unknown,
										"Search was aborted.");
		}

		State* s = stack.back();
		stack.pop_back();
		State* succ[net.numberOfTransitions()];
		double distances[net.numberOfTransitions()];
		memset(succ, 0, net.numberOfTransitions()*sizeof(State*));
		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s, ns)){
				if(states.add(ns)){
					exploredStates++;
					ns->setParent(s);
					ns->setTransition(t);
					if(query->evaluate(*ns))
						return ReachabilityResult(ReachabilityResult::Satisfied,
												"A state satisfying the query was found", expandedStates, exploredStates, ns->pathLength(), ns->trace());
					PQL::DistanceContext context(net,
												 _distanceStrategy,
												 ns->marking(),
												 ns->valuation(),
												 &distanceMatrix);
					succ[t] = ns;
					distances[t] = query->distance(context);
					ns = allocator.createState();
				}
			}
		}

		// Pretty bobble sort of the successors
		while(true){
			bool foundSomething = false;
			unsigned int min = 0;
			for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
				if(succ[t] && (!foundSomething || distances[t] < distances[min])){
					min = t;
					foundSomething = true;
				}
			}
			if(foundSomething){
				stack.push_back(succ[min]);
				succ[min] = NULL;
			}else
				break;
		}
		expandedStates++;
	}

	return ReachabilityResult(ReachabilityResult::NotSatisfied,
						"No state satisfying the query exists.", expandedStates, exploredStates);
}

} // Reachability
} // PetriEngine
