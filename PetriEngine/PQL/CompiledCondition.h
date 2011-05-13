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
