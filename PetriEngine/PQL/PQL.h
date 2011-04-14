#ifndef PQL_H
#define PQL_H
#include "../PetriNet.h"
#include "../Structures/State.h"
#include <string>
#include <list>
#include <vector>

namespace PetriEngine{
namespace PQL{

	/** Representation of a PQL error */
class ExprError{
	std::string _text;
	int _offset;
	int _length;
public:
	ExprError(std::string text = "", int offset = -1, int length = 0){
		_text = text;
		_offset = offset;
		_length = length;
	}
	/** Human readable explaination of the error */
	const std::string& text() const {return _text;}
	/** Offset in the source, -1 if undefined */
	int offset() const {return _offset;}
	/** length in the source, 0 if not applicable */
	int length() const {return _length;}

	/** Convert error to string */
	std::string toString() const {
		return "Parsing error \"" + text() + "\"";
	}
	/** True, if this is a default created ExprError without any information */
	bool isEmpty() const {
		return _text.empty() && _offset == -1 && _length == 0;
	}
};

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

/** Representation of an expression */
class Expr{
public:
	/** Virtual destructor, an expression should know it subexpressions */
	virtual ~Expr();
	/** Evaluate the expression given marking and assignment */
	virtual int evaluate(const EvaluationContext& context) const = 0;
	/** Perform context analysis */
	virtual void analyze(AnalysisContext& context) = 0;
	/** Convert expression to string */
	virtual std::string toString() const = 0;
};

/** Base condition */
class Condition{
public:
	/** Strategy flags for distance computation */
	enum DistanceStrategy{
		AndExtreme	= 0x1,
		AndAverage	= 0x2,
		OrExtreme	= 0x4,
		OrAverage	= 0x8
	};

	/** Virtual destructor */
	virtual ~Condition();

	/** Evaluate condition */
	bool evaluate(Structures::State& state) const{
		return evaluate(EvaluationContext(state.marking(), state.valuation()));
	}

	/** Evaluate condition */
	virtual bool evaluate(const EvaluationContext& context) const = 0;
	/** Perform context analysis  */
	virtual void analyze(AnalysisContext& context) = 0;
	/** Convert condition to string */
	virtual std::string toString() const = 0;
	/** Get distance to query */
	virtual double distance(const EvaluationContext& context,
							DistanceStrategy strategy,
							bool negated = false) const = 0;
};

/** Assignment expression */
class AssignmentExpression{
private:
	struct VariableAssignment{
		std::string identifier;
		int offset;
		Expr* expr;
	};
	typedef std::list<VariableAssignment>::iterator iter;
	typedef std::list<VariableAssignment>::const_iterator const_iter;
public:
	void prepend(const std::string& identifier, Expr* expr){
		VariableAssignment va;
		va.offset = -1;
		va.identifier = identifier;
		va.expr = expr;
		assignments.push_front(va);
	}
	void analyze(AnalysisContext& context){
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
	/** Evaluate the assignment expression */
	void evaluate(const MarkVal* m,
				  const VarVal* a,
				  VarVal* result_a,
				  VarVal* ranges,
				  size_t nvars) const;
	std::string toString(){
		std::string t;
		for(iter it = assignments.begin(); it != assignments.end(); it++){
			t = t + it->identifier + " := " + it->expr->toString() + "; ";
		}
		return t;
	}
private:
	std::list<VariableAssignment> assignments;
};

}}

#endif // PQL_H
