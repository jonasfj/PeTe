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
#include "StateSearch.h"

#include "../Structures/EnhancedPriorityQueue.h"
#include "../Structures/StateSet.h"
#include "../Structures/BoundedStateAllocator.h"
#include "../PQL/Contexts.h"
#include "../Structures/DistanceMatrix.h"

#include <string.h>
#include <limits.h>

using namespace PetriEngine::Structures;
using namespace PetriEngine::PQL;

/** Define memory bound for ultimate search strategy */
#define ULTIMATE_MEMORY_BOUND				1024*1024*1024

namespace PetriEngine{
namespace Reachability{

ReachabilityResult StateSearch::reachable(const PetriNet &net,
										  const MarkVal *m0,
										  const VarVal *v0,
										  PQL::Condition *query){
	// Test if it initally satisfied
	{
		if(query->evaluate(EvaluationContext(m0, v0)))
			return ReachabilityResult(ReachabilityResult::Satisfied, "Satisifed initially", 0, 0, 0);
	}

	// Attempt to exclude by over-approx
	ConstraintAnalysisContext context(net);
	query->findConstraints(context);
	if(!context.canAnalyze)
		return ReachabilityResult(ReachabilityResult::Unknown,
											  "Can't analyze",
											  0, 0);
	bool isImpossible = true;
	for(size_t i = 0; i < context.retval.size(); i++){
		isImpossible &= context.retval[i]->isImpossible(net, m0, v0);
		if(!isImpossible) break;
	}
	if(isImpossible)
		return ReachabilityResult(ReachabilityResult::NotSatisfied,
								  "Excluded by over-approximation",
								  0, 0, 0);


	// Create allocator, stateset and queue
	BoundedStateAllocator<ULTIMATE_MEMORY_BOUND> allocator(net);
	StateSet states(net);
	EnhancedPriorityQueue<State*> queue;

	// Create s0
	State s0;
	s0.setParent(NULL);
	s0.setTransition(0);
	s0.setMarking(const_cast<MarkVal*>(m0));
	s0.setValuation(const_cast<VarVal*>(v0));

	// Push s0
	queue.push(1, &s0);
	states.add(&s0);

	// Statistics
	BigInt expanded = 0, explored = 0;
	size_t max = 1;
	int count = 0;

	State* ns = allocator.createState();

	// Main-loop
	while(!queue.empty()){
		// Report progress
		if(count++ & 1<<17){
			count = 0;
			if(queue.size() > max)
				max = queue.size();
			this->reportProgress((double)(max - queue.size()) / ((double)(max)));
			if(this->abortRequested())
				return ReachabilityResult(ReachabilityResult::Unknown,
										  "Query aborted!");
		}

		// Pop from queue
		State* s = queue.pop(depthFirst);
		expanded++;

		// Try firing each transition
		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s, ns) && states.add(ns)){
				// Update statistics
				explored++;
				ns->setParent(s);
				ns->setTransition(t);

				// Test query
				if(query->evaluate(*ns)){
					return ReachabilityResult(ReachabilityResult::Satisfied,
											  "Query was satisfied",
											  expanded, explored, ns->pathLength(), ns->trace());
				}

				bool isImpossible = true;
				int smallest = INT_MAX;
				for(size_t i = 0; i < context.retval.size(); i++){
					int size = context.retval[i]->fireVectorSize(net, m0, v0);
					if(size > 0)
						isImpossible = false;
					if(smallest > size)
						smallest = size;
				}
				if(isImpossible)
					return ReachabilityResult(ReachabilityResult::NotSatisfied,
											  "Excluded by over-approximation",
											  0, 0, 0);
				queue.push(smallest, ns);

				// Allocate new state, abort if not possible
				ns = allocator.createState();
				if(!ns)
					return ReachabilityResult(ReachabilityResult::Unknown,
											  "Memory bound exceeded",
											  expanded, explored);
			}
		}
	}

	return ReachabilityResult(ReachabilityResult::NotSatisfied,
							  "Query cannot be satisfied",
							  expanded, explored);
}

} // Reachability
} // PetriEngine

