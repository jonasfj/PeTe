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
#ifndef CONTEXTS_H
#define CONTEXTS_H

#include "../PetriNet.h"
#include "PQL.h"
#include "../Structures/DistanceMatrix.h"
#include "../Structures/StateConstraints.h"

#include <string>
#include <vector>
#include <list>

namespace PetriEngine {
namespace PQL{

/** Context provided for context analysis */
class AnalysisContext{
private:
	std::vector<std::string> _places;
	std::vector<std::string> _variables;
	std::vector<ExprError> _errors;
public:
	/** A resolution result */
	struct ResolutionResult {
		/** Offset in relevant vector */
		int offset;
		/** True, if the resolution was successful */
		bool success;
		/** True if the identifer was resolved to a place */
		bool isPlace;
	};
	AnalysisContext(const PetriNet& net)
	 : _places(net.placeNames()), _variables(net.variableNames()) {}
	AnalysisContext(const std::vector<std::string>& places,
					const std::vector<std::string>& variables)
	 : _places(places), _variables(variables) {}

	/** Resolve an identifier */
	ResolutionResult resolve(std::string identifier) const{
		ResolutionResult result;
		result.offset = -1;
		result.success = false;
		for(size_t i = 0; i < _places.size(); i++){
			if(_places[i] == identifier){
				result.offset = i;
				result.isPlace = true;
				result.success = true;
				return result;
			}
		}
		for(size_t i = 0; i < _variables.size(); i++){
			if(_variables[i] == identifier){
				result.offset = i;
				result.isPlace = false;
				result.success = true;
				return result;
			}
		}
		return result;
	}

	/** Report error */
	void reportError(const ExprError& error){
		_errors.push_back(error);
	}
	/** Get list of errors */
	const std::vector<ExprError>& errors() const{
		return _errors;
	}
};

/** Context provided for evalation */
class EvaluationContext{
public:
	/** Create evaluation context, this doesn't take ownershop */
	EvaluationContext(const MarkVal* marking, const VarVal* assignment){
		_marking = marking;
		_assignment = assignment;
	}
	const MarkVal* marking() const {return _marking;}
	const VarVal* assignment() const {return _assignment;}
private:
	const MarkVal* _marking;
	const VarVal* _assignment;
};

/** Context for distance computation */
class DistanceContext : public EvaluationContext{
public:
	/** Strategy flags for distance computation */
	enum DistanceStrategy{
		AndExtreme	= 1<<0,
		AndAverage	= 1<<1,
		AndSum		= 1<<2,
		OrExtreme	= 1<<3,
		OrAverage	= 1<<4,
		ArcCount	= 1<<5,
		TokenCost	= 1<<6
	};

	DistanceContext(const PetriNet& net,
					DistanceStrategy strategy,
					const MarkVal* marking,
					const VarVal* valuation,
					Structures::DistanceMatrix* dm)
		: EvaluationContext(marking, valuation), _net(net) {
		_strategy = strategy;
		_negated = false;
		_dm = dm;
	}
	DistanceStrategy strategy() const { return _strategy; }
	const PetriNet& net() const { return _net; }
	void negate() { _negated = !_negated; }
	bool negated() const { return _negated; }
	const Structures::DistanceMatrix* distanceMatrix() const { return _dm; }
private:
	const PetriNet& _net;
	DistanceStrategy _strategy;
	bool _negated;
	Structures::DistanceMatrix* _dm;
};

/** Constraint Analysis Context used for over-approximation */
class ConstraintAnalysisContext{
public:
	typedef std::vector<Structures::StateConstraints*> ConstraintSet;

	ConstraintAnalysisContext(const PetriNet& net) : _net(net) {
		canAnalyze = true;
		negated = false;
	}
	const PetriNet& net() const { return _net; }
	bool canAnalyze;
	bool negated;
	ConstraintSet retval;
private:
	const PetriNet& _net;
};

/** Just-In-Time compilation context */
class CodeGenerationContext{
public:
	CodeGenerationContext(llvm::Value* marking,
						  llvm::Value* valuation,
						  llvm::BasicBlock* label,
						  llvm::LLVMContext& context)
		: _context(context) {
		_marking = marking;
		_valuation = valuation;
		_label = label;
	}
	/** Marking */
	llvm::Value* marking() { return _marking; }
	/** Variable valuation */
	llvm::Value* valuation() { return _valuation; }
	/** Label for the current code block */
	llvm::BasicBlock* label() { return _label; }
	/** LLVM Context that is currently generating */
	llvm::LLVMContext& context() { return _context; }
private:
	llvm::Value* _marking;
	llvm::Value* _valuation;
	llvm::BasicBlock* _label;
	llvm::LLVMContext& _context;
};

} // PQL
} // PetriEngine

#endif // CONTEXTS_H
