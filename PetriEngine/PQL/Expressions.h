#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "PQL.h"

namespace PetriEngine {
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
	int evaluate(const EvaluationContext& context) const;
	std::string toString() const;
private:
	virtual int apply(int v1, int v2) const = 0;
	virtual std::string op() const = 0;
	Expr* _expr1;
	Expr* _expr2;
};

/** Binary plus expression */
class PlusExpr : public BinaryExpr{
public:
	PlusExpr(Expr* expr1, Expr* expr2) : BinaryExpr(expr1, expr2) {}
private:
	int apply(int v1, int v2) const;
	std::string op() const;
};

/** Binary minus expression */
class SubtractExpr : public BinaryExpr{
public:
	SubtractExpr(Expr* expr1, Expr* expr2) : BinaryExpr(expr1, expr2) {}
private:
	int apply(int v1, int v2) const;
	std::string op() const;
};

/** Binary multiplication expression **/
class MultiplyExpr : public BinaryExpr{
public:
	MultiplyExpr(Expr* expr1, Expr* expr2) : BinaryExpr(expr1, expr2) {}
private:
	int apply(int v1, int v2) const;
	std::string op() const;
};

/** Unary minus expression*/
class MinusExpr : public Expr {
public:
	MinusExpr(Expr* expr){
		_expr = expr;
	}
	~MinusExpr();
	int evaluate(const EvaluationContext& context) const;
	void analyze(AnalysisContext& context);
	std::string toString() const;
private:
	Expr* _expr;
};

/** Literal integer value expression */
class LiteralExpr : public Expr {
public:
	LiteralExpr(int value) : _value(value){}
	int evaluate(const EvaluationContext& context) const;
	void analyze(AnalysisContext& context);
	std::string toString() const;
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
	int evaluate(const EvaluationContext& context) const;
	void analyze(AnalysisContext& context);
	std::string toString() const;
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
	bool evaluate(const EvaluationContext& context) const;
	void analyze(AnalysisContext& context);
	std::string toString() const;
private:
	virtual bool apply(bool b1, bool b2) const = 0;
	virtual std::string op() const = 0;
	Condition* _cond1;
	Condition* _cond2;
};

/* Conjunctive and condition */
class AndCondition : public LogicalCondition{
public:
	AndCondition(Condition* cond1, Condition* cond2) : LogicalCondition(cond1,cond2) {}
private:
	bool apply(bool b1, bool b2) const;
	std::string op() const;
};

/* Disjunctive or conditon */
class OrCondition : public LogicalCondition{
public:
	OrCondition(Condition* cond1, Condition* cond2) : LogicalCondition(cond1,cond2) {}
private:
	bool apply(bool b1, bool b2) const;
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
	bool evaluate(const EvaluationContext& context) const;
	void analyze(AnalysisContext& context);
	std::string toString() const;
private:
	virtual bool apply(int v1, int v2) const = 0;
	virtual std::string op() const = 0;
	Expr* _expr1;
	Expr* _expr2;
};

/* Equality conditon */
class EqualCondition : public CompareCondition{
public:
	EqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std::string op() const;
};

/* None equality conditon */
class NotEqualCondition : public CompareCondition{
public:
	NotEqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std::string op() const;
};

/* Less-than conditon */
class LessThanCondition : public CompareCondition{
public:
	LessThanCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std::string op() const;
};

/* Less-than-or-equal conditon */
class LessThanOrEqualCondition : public CompareCondition{
public:
	LessThanOrEqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std::string op() const;
};

/* Greater-than conditon */
class GreaterThanCondition : public CompareCondition{
public:
	GreaterThanCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std::string op() const;
};

/* Greater-than-or-equal conditon */
class GreaterThanOrEqualCondition : public CompareCondition{
public:
	GreaterThanOrEqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std::string op() const;
};

/* Not condition */
class NotCondition : public Condition{
public:
	NotCondition(Condition* cond){
		_cond = cond;
	}
	~NotCondition();
	bool evaluate(const EvaluationContext& context) const;
	void analyze(AnalysisContext& context);
	std::string toString() const;
private:
	Condition* _cond;
};

}}



#endif // EXPRESSIONS_H
