#ifndef COMPILEDCONDITION_H
#define COMPILEDCONDITION_H

#include "PQL.h"

namespace llvm{
	class ExecutionEngine;
};

namespace PetriEngine{
namespace PQL{

/** Just-In-Time compiled condition */
class CompiledCondition : public Condition
{
public:
	CompiledCondition(Condition* condition){
		_cond = condition;
		_engine = NULL;
		_nativeFunction = NULL;
	}
	~CompiledCondition();
	void analyze(AnalysisContext &context);
	bool compile();
	llvm::Value* codegen(CodeGenerationContext &context) const;
	bool evaluate(const EvaluationContext &context) const;
	void findConstraints(ConstraintAnalysisContext& context) const;
	double distance(DistanceContext& context) const;
	std::string toString() const;
	void scale(int factor);
private:
	Condition* _cond;
	llvm::ExecutionEngine* _engine;
	bool(*_nativeFunction)(const MarkVal*, const VarVal*);
};

} // PQL
} // PetriEngine

#endif // COMPILEDCONDITION_H
