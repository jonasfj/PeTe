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
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "PQL.h"
#include "Contexts.h"

namespace PetriEngine {

namespace Structures {
	class StateConstraints;
}

namespace PQL {


/******************** EXPRESSIONS ********************/

/** Base class for all binary expressions */
class BinaryExpr : public Expr{
public:
	BinaryExpr(Expr* expr1, Expr* expr2){
		_expr1 = expr1;
		_expr2 = expr2;
	}
	~BinaryExpr();
	void analyze(AnalysisContext& context);
	bool pfree() const;
	int evaluate(const EvaluationContext& context) const;
	llvm::Value* codegen(CodeGenerationContext& context) const;
	std::string toString() const;
	void scale(int factor);
private:
	virtual int apply(int v1, int v2) const = 0;
	/** LLVM binary operator (llvm::Instruction::BinaryOps) */
	virtual int binaryOp() const = 0;
	virtual std::string op() const = 0;
	Expr* _expr1;
	Expr* _expr2;
};

/** Binary plus expression */
class PlusExpr : public BinaryExpr{
public:
	PlusExpr(Expr* expr1, Expr* expr2) : BinaryExpr(expr1, expr2) {}
	Expr::Types type() const;
private:
	int apply(int v1, int v2) const;
	int binaryOp() const;
	std::string op() const;
};

/** Binary minus expression */
class SubtractExpr : public BinaryExpr{
public:
	SubtractExpr(Expr* expr1, Expr* expr2) : BinaryExpr(expr1, expr2) {}
	Expr::Types type() const;
private:
	int apply(int v1, int v2) const;
	int binaryOp() const;
	std::string op() const;
};

/** Binary multiplication expression **/
class MultiplyExpr : public BinaryExpr{
public:
	MultiplyExpr(Expr* expr1, Expr* expr2) : BinaryExpr(expr1, expr2) {}
	Expr::Types type() const;
private:
	int apply(int v1, int v2) const;
	int binaryOp() const;
	std::string op() const;
};

/** Unary minus expression*/
class MinusExpr : public Expr {
public:
	MinusExpr(Expr* expr){
		_expr = expr;
	}
	~MinusExpr();
	void analyze(AnalysisContext& context);
	bool pfree() const;
	int evaluate(const EvaluationContext& context) const;
	llvm::Value* codegen(CodeGenerationContext& context) const;
	std::string toString() const;
	Expr::Types type() const;
	void scale(int factor);
private:
	Expr* _expr;
};

/** Literal integer value expression */
class LiteralExpr : public Expr {
public:
	LiteralExpr(int value) : _value(value){}
	void analyze(AnalysisContext& context);
	bool pfree() const;
	int evaluate(const EvaluationContext& context) const;
	llvm::Value* codegen(CodeGenerationContext& context) const;
	std::string toString() const;
	Expr::Types type() const;
	int value() const { return _value; };
	void scale(int factor);
private:
	int _value;
};

/** Identifier expression */
class IdentifierExpr : public Expr {
public:
	IdentifierExpr(const std::string& name, int srcOffset) : _name(name) {
		_offsetInMarking = -1;
		_srcOffset = srcOffset;
	}
	void analyze(AnalysisContext& context);
	bool pfree() const;
	int evaluate(const EvaluationContext& context) const;
	llvm::Value* codegen(CodeGenerationContext& context) const;
	std::string toString() const;
	Expr::Types type() const;
	/** Offset in marking or valuation */
	int offset() const{ return _offsetInMarking; }
	void scale(int factor);
private:
	/** Is this identifier a place? Or a variable.. */
	bool isPlace;
	/** Offset in marking, -1 if undefined, should be resolved during analysis */
	int _offsetInMarking;
	/** Offset in source, as provided to parser */
	int _srcOffset;
	/** Identifier text */
	std::string _name;
};

/******************** CONDITIONS ********************/

/* Logical conditon */
class LogicalCondition : public Condition{
public:
	LogicalCondition(Condition* cond1, Condition* cond2){
		_cond1 = cond1;
		_cond2 = cond2;
	}
	~LogicalCondition();
	void analyze(AnalysisContext& context);
	bool evaluate(const EvaluationContext& context) const;
	void findConstraints(ConstraintAnalysisContext& context) const;
	llvm::Value* codegen(CodeGenerationContext& context) const;
	double distance(DistanceContext& context) const;
	std::string toString() const;
	void scale(int factor);
private:
	virtual bool apply(bool b1, bool b2) const = 0;
	/** LLVM binary operator (llvm::Instruction::BinaryOps) */
	virtual int logicalOp() const = 0;
	virtual double delta(double d1, double d2, const DistanceContext& context) const = 0;
	virtual std::string op() const = 0;
	virtual void mergeConstraints(ConstraintAnalysisContext::ConstraintSet& result, ConstraintAnalysisContext::ConstraintSet& other, bool negated) const = 0;
	Condition* _cond1;
	Condition* _cond2;
};

/* Conjunctive and condition */
class AndCondition : public LogicalCondition{
public:
	AndCondition(Condition* cond1, Condition* cond2) : LogicalCondition(cond1,cond2) {}
private:
	bool apply(bool b1, bool b2) const;
	int logicalOp() const;
	double delta(double d1, double d2, const DistanceContext& context) const;
	void mergeConstraints(ConstraintAnalysisContext::ConstraintSet& result, ConstraintAnalysisContext::ConstraintSet& other, bool negated) const;
	std::string op() const;
};

/* Disjunctive or conditon */
class OrCondition : public LogicalCondition{
public:
	OrCondition(Condition* cond1, Condition* cond2) : LogicalCondition(cond1,cond2) {}
private:
	bool apply(bool b1, bool b2) const;
	int logicalOp() const;
	double delta(double d1, double d2, const DistanceContext& context) const;
	void mergeConstraints(ConstraintAnalysisContext::ConstraintSet& result, ConstraintAnalysisContext::ConstraintSet& other, bool negated) const;
	std::string op() const;
};

/* Comparison conditon */
class CompareCondition : public Condition{
public:
	CompareCondition(Expr* expr1, Expr* expr2){
		_expr1 = expr1;
		_expr2 = expr2;
	}
	~CompareCondition();
	void analyze(AnalysisContext& context);
	bool evaluate(const EvaluationContext& context) const;
	void findConstraints(ConstraintAnalysisContext& context) const;
	llvm::Value* codegen(CodeGenerationContext& context) const;
	double distance(DistanceContext& context) const;
	std::string toString() const;
	void scale(int factor);
private:
	virtual bool apply(int v1, int v2) const = 0;
	/** LLVM Comparison predicate (llvm::ICmpInst::Predicate) */
	virtual int compareOp() const = 0;
	virtual double delta(int v1, int v2, bool negated) const = 0;
	virtual std::string op() const = 0;
	virtual void addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const = 0;
	virtual void addConstraints(ConstraintAnalysisContext& context,	int value, IdentifierExpr* id) const = 0;
	Expr* _expr1;
	Expr* _expr2;
};

/* Equality conditon */
class EqualCondition : public CompareCondition{
public:
	EqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	int compareOp() const;
	double delta(int v1, int v2, bool negated) const;
	void addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const;
	void addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const;
	std::string op() const;
};

/* None equality conditon */
class NotEqualCondition : public CompareCondition{
public:
	NotEqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	int compareOp() const;
	double delta(int v1, int v2, bool negated) const;
	void addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const;
	void addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const;
	std::string op() const;
};

/* Less-than conditon */
class LessThanCondition : public CompareCondition{
public:
	LessThanCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	int compareOp() const;
	double delta(int v1, int v2, bool negated) const;
	void addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const;
	void addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const;
	std::string op() const;
};

/* Less-than-or-equal conditon */
class LessThanOrEqualCondition : public CompareCondition{
public:
	LessThanOrEqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	int compareOp() const;
	double delta(int v1, int v2, bool negated) const;
	void addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const;
	void addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const;
	std::string op() const;
};

/* Greater-than conditon */
class GreaterThanCondition : public CompareCondition{
public:
	GreaterThanCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	int compareOp() const;
	double delta(int v1, int v2, bool negated) const;
	void addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const;
	void addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const;
	std::string op() const;
};

/* Greater-than-or-equal conditon */
class GreaterThanOrEqualCondition : public CompareCondition{
public:
	GreaterThanOrEqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	int compareOp() const;
	double delta(int v1, int v2, bool negated) const;
	void addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const;
	void addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const;
	std::string op() const;
};

/* Not condition */
class NotCondition : public Condition{
public:
	NotCondition(Condition* cond){
		_cond = cond;
	}
	~NotCondition();
	void analyze(AnalysisContext& context);
	bool evaluate(const EvaluationContext& context) const;
	void findConstraints(ConstraintAnalysisContext& context) const;
	llvm::Value* codegen(CodeGenerationContext& context) const;
	double distance(DistanceContext& context) const;
	std::string toString() const;
	void scale(int factor);
private:
	Condition* _cond;
};

}}



#endif // EXPRESSIONS_H
