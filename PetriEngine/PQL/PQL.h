#ifndef PQL_H
#define PQL_H
#include "../PetriNet.h"
#include "../Structures/State.h"
#include <string>
#include <list>
#include <vector>

namespace llvm{
	class Value;
	class BasicBlock;
	class LLVMContext;
}

namespace PetriEngine{
namespace PQL{

class AnalysisContext;
class EvaluationContext;
class DistanceContext;
class CodeGenerationContext;

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



/** Representation of an expression */
class Expr{
public:
	/** Types of expressions */
	enum Types{
		/** Binary addition expression */
		PlusExpr,
		/** Binary subtraction expression */
		SubtractExpr,
		/** Binary multiplication expression */
		MultiplyExpr,
		/** Unary minus expression */
		MinusExpr,
		/** Literal integer expression */
		LiteralExpr,
		/** Identifier expression */
		IdentifierExpr
	};
public:
	/** Virtual destructor, an expression should know it subexpressions */
	virtual ~Expr();
	/** Perform context analysis */
	virtual void analyze(AnalysisContext& context) = 0;
	/** True, if the expression is p-free */
	virtual bool pfree() const = 0;
	/** Evaluate the expression given marking and assignment */
	virtual int evaluate(const EvaluationContext& context) const = 0;
	/** Generate LLVM intermediate code for this expr  */
	virtual llvm::Value* codegen(CodeGenerationContext& context) const = 0;
	/** Convert expression to string */
	virtual std::string toString() const = 0;
	/** Expression type */
	virtual Types type() const = 0;
};

/** Base condition */
class Condition{
public:
	/** Virtual destructor */
	virtual ~Condition();
	/** Evaluate condition */
	bool evaluate(Structures::State& state) const;
	/** Perform context analysis  */
	virtual void analyze(AnalysisContext& context) = 0;
	/** Evaluate condition */
	virtual bool evaluate(const EvaluationContext& context) const = 0;
	/** Generate LLVM intermediate code for this condition  */
	virtual llvm::Value* codegen(CodeGenerationContext& context) const = 0;
	/** Convert condition to string */
	virtual std::string toString() const = 0;
	/** Get distance to query */
	virtual double distance(DistanceContext& context) const = 0;
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
	void analyze(AnalysisContext& context);
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

} // PQL
} // PetriEngine

#endif // PQL_H
