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
