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
#include "PQL.h"

#include "Contexts.h"

namespace PetriEngine {
namespace PQL {

Expr::~Expr(){}

bool Condition::evaluate(Structures::State &state) const{
	return evaluate(EvaluationContext(state.marking(), state.valuation()));
}

Condition::~Condition(){}


void AssignmentExpression::analyze(AnalysisContext& context){
	for(iter it = assignments.begin(); it != assignments.end(); it++){
		AnalysisContext::ResolutionResult result = context.resolve(it->identifier);
		if(result.success && !result.isPlace){
			it->offset = result.offset;
		}else if(result.isPlace){
			context.reportError(ExprError("You cannot assign to an place!"));
		}else{
			context.reportError(ExprError("Variable for assignment could not be resolved!"));
		}
		it->expr->analyze(context);
	}
}

} // PQL
} // PetriEngine
