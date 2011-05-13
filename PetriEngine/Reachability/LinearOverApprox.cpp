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
#include "LinearOverApprox.h"
#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"

#include <assert.h>

namespace PetriEngine{
namespace Reachability{

ReachabilityResult LinearOverApprox::reachable(const PetriNet &net,
											   const MarkVal *m0,
											   const VarVal *ia,
											   PQL::Condition *query){
	PQL::ConstraintAnalysisContext context(net);
	query->findConstraints(context);

	if(context.canAnalyze){
		// Try each possible linear StateConstraints
		bool isImpossible = true;
		for(size_t i = 0; i < context.retval.size(); i++){
			isImpossible &= context.retval[i]->isImpossible(net, m0, ia);
			if(!isImpossible) break;
		}
		assert(context.retval.size() > 0);

		if(isImpossible){
			return ReachabilityResult(ReachabilityResult::NotSatisfied,
						"Query proved not satisfiable by over-approximation");
		}
	}

	// Release anything we got from ConstraintAnalysis
	for(size_t i = 0; i < context.retval.size(); i++)
		delete context.retval[i];

	// Try fallback strategy if there's one
	if(fallback){
		fallback->setProgressReporter(this->reporter());
		return fallback->reachable(net, m0, ia, query);
	}

	// If there's complex expression we can't do anything
	if(!context.canAnalyze){
		return ReachabilityResult(ReachabilityResult::Unknown,
			"Expressions are too complex for constraint analysis");
	}

	return ReachabilityResult(ReachabilityResult::Unknown,
			"Couldn't exclude query by over-approximation");
}

} // Reachability
} // PetriEngine
