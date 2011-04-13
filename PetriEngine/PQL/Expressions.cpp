#include "Expressions.h"

#include <sstream>
#include <assert.h>
#include <string.h>

namespace PetriEngine {
namespace PQL{

/******************** Destructors ********************/

BinaryExpr::~BinaryExpr(){
	if(_expr1)
		delete _expr1;
	_expr1 = NULL;
	if(_expr2)
		delete _expr2;
	_expr2 = NULL;
}

MinusExpr::~MinusExpr(){
	if(_expr)
		delete _expr;
	_expr = NULL;
}

LogicalCondition::~LogicalCondition(){
	if(_cond1)
		delete _cond1;
	_cond1 = NULL;
	if(_cond2)
		delete _cond2;
	_cond2 = NULL;
}

CompareCondition::~CompareCondition(){
	if(_expr1)
		delete _expr1;
	_expr1 = NULL;
	if(_expr2)
		delete _expr2;
	_expr2 = NULL;
}

NotCondition::~NotCondition(){
	if(_cond)
		delete _cond;
	_cond = NULL;
}

/******************** To String ********************/

std::string LiteralExpr::toString() const{
	std::stringstream stream;
	stream <<_value;
	return stream.str();
}

std::string IdentifierExpr::toString() const{
	return _name;
}

std::string BinaryExpr::toString() const{
	return "(" + _expr1->toString() + " " + op() + " " + _expr2->toString() + ")";
}

std::string MinusExpr::toString() const{
	return "-" + _expr->toString();
}

std::string LogicalCondition::toString() const{
	return "(" + _cond1->toString() + " " + op() + " " + _cond2->toString() + ")";
}

std::string CompareCondition::toString() const{
	return "(" + _expr1->toString() + " " + op() + " " + _expr2->toString() + ")";
}

std::string NotCondition::toString() const {
	return "!" + _cond->toString();
}

/******************** Context Analysis ********************/

void BinaryExpr::analyze(AnalysisContext& context){
	_expr1->analyze(context);
	_expr2->analyze(context);
}

void MinusExpr::analyze(AnalysisContext& context){
	_expr->analyze(context);
}

void LiteralExpr::analyze(AnalysisContext&){
	return;
}

void IdentifierExpr::analyze(AnalysisContext& context){
	AnalysisContext::ResolutionResult result = context.resolve(_name);
	if(result.success){
		_offsetInMarking = result.offset;
		isPlace = result.isPlace;
	}
	else{
		ExprError error("Unable to resolve identifier \"" + _name + "\"",
						_srcOffset,
						_name.length());
		context.reportError(error);
	}
}

void LogicalCondition::analyze(AnalysisContext& context){
	_cond1->analyze(context);
	_cond2->analyze(context);
}

void CompareCondition::analyze(AnalysisContext& context){
	_expr1->analyze(context);
	_expr2->analyze(context);
}

void NotCondition::analyze(AnalysisContext &context){
	_cond->analyze(context);
}

/******************** Evaluation ********************/

int BinaryExpr::evaluate(const EvaluationContext& context) const{
	int v1 = _expr1->evaluate(context);
	int v2 = _expr2->evaluate(context);
	return apply(v1, v2);
}

int MinusExpr::evaluate(const EvaluationContext& context) const{
	return -(_expr->evaluate(context));
}

int LiteralExpr::evaluate(const EvaluationContext&) const{
	return _value;
}

int IdentifierExpr::evaluate(const EvaluationContext& context) const{
	assert(_offsetInMarking != -1);
	if(isPlace)
		return context.marking()[_offsetInMarking];
	return context.assignment()[_offsetInMarking];
}

bool LogicalCondition::evaluate(const EvaluationContext& context) const{
	bool b1 = _cond1->evaluate(context);
	bool b2 = _cond2->evaluate(context);
	return apply(b1,b2);
}

bool CompareCondition::evaluate(const EvaluationContext& context) const{
	int v1 = _expr1->evaluate(context);
	int v2 = _expr2->evaluate(context);
	return apply(v1,v2);
}


bool NotCondition::evaluate(const EvaluationContext& context) const{
	return !(_cond->evaluate(context));
}

void AssignmentExpression::evaluate(const MarkVal* m,
									const VarVal *a,
									VarVal *result_a,
									VarVal* ranges,
									size_t nvars) const{
	memcpy(result_a, a, sizeof(VarVal) * nvars);
	EvaluationContext context(m, a);
	for(const_iter it = assignments.begin(); it != assignments.end(); it++)
		result_a[it->offset] = it->expr->evaluate(context) % ranges[it->offset];
}

/******************** Apply (BinaryExpr subclasses) ********************/

int PlusExpr::apply(int v1, int v2) const{
	return v1 + v2;
}

int SubtractExpr::apply(int v1, int v2) const{
	return v1 - v2;
}

int MultiplyExpr::apply(int v1, int v2) const{
	return v1 * v2;
}

/******************** Apply (LogicalCondition subclasses) ********************/

bool AndCondition::apply(bool b1, bool b2) const{
	return b1 && b2;
}

bool OrCondition::apply(bool b1, bool b2) const{
	return b1 || b2;
}

/******************** Apply (CompareCondition subclasses) ********************/

bool EqualCondition::apply(int v1, int v2) const{
	return v1 == v2;
}

bool NotEqualCondition::apply(int v1, int v2) const{
	return v1 != v2;
}

bool LessThanCondition::apply(int v1, int v2) const{
	return v1 < v2;
}

bool LessThanOrEqualCondition::apply(int v1, int v2) const{
	return v1 <= v2;
}

bool GreaterThanCondition::apply(int v1, int v2) const{
	return v1 > v2;
}

bool GreaterThanOrEqualCondition::apply(int v1, int v2) const{
	return v1 >= v2;
}
/******************** Op (BinaryExpr subclasses) ********************/

std::string PlusExpr::op() const{
	return "+";
}

std::string SubtractExpr::op() const{
	return "-";
}

std::string MultiplyExpr::op() const{
	return "*";
}

/******************** Op (LogicalCondition subclasses) ********************/

std::string AndCondition::op() const{
	return "and";
}

std::string OrCondition::op() const{
	return "or";
}

/******************** Op (CompareCondition subclasses) ********************/
std::string EqualCondition::op() const{
	return "==";
}

std::string NotEqualCondition::op() const{
	return "!=";
}

std::string LessThanCondition::op() const{
	return "<";
}

std::string LessThanOrEqualCondition::op() const{
	return "<=";
}

std::string GreaterThanCondition::op() const{
	return ">";
}

std::string GreaterThanOrEqualCondition::op() const{
	return ">=";
}


/******************** Distance Condition ********************/

#define MAX(v1, v2)		(v1 > v2 ? v1 : v2)
#define MIN(v1, v2)		(v1 > v2 ? v1 : v2)

double NotCondition::distance(const EvaluationContext& context,
							  DistanceStrategy strategy,
							  bool negated) const{
	return _cond->distance(context, strategy, !negated);
}

double LogicalCondition::distance(const EvaluationContext& context,
								  DistanceStrategy strategy,
								  bool negated) const{
	double d1 = _cond1->distance(context, strategy, negated);
	double d2 = _cond2->distance(context, strategy, negated);
	return distance(d1, d2, strategy, negated);
}

double AndCondition::distance(double d1,
							  double d2,
							  DistanceStrategy strategy,
							  bool negated) const{
	if(strategy & Condition::AndExtreme)
		if(negated)
			return MIN(d1, d2);
		else
			return MAX(d1, d2);
	else
		return (d1 + d2) / 2;
}

double OrCondition::distance(double d1,
							 double d2,
							 DistanceStrategy strategy,
							 bool negated) const{
	if(strategy & Condition::OrExtreme)
		if(negated)
			return MAX(d1, d2);
		else
			return MIN(d1, d2);
	else
		return (d1 + d2) / 2;
}

double CompareCondition::distance(const EvaluationContext& context,
								  DistanceStrategy,
								  bool negated) const{
	int v1 = _expr1->evaluate(context);
	int v2 = _expr2->evaluate(context);
	return distance(v1, v2, negated);
}

double EqualCondition::distance(int v1, int v2, bool negated) const{
	if(!negated)
		return v1 - v2;
	else
		return v1 == v2 ? 1 : 0;
}

double NotEqualCondition::distance(int v1, int v2, bool negated) const{
	if(negated)
		return v1 - v2;
	else
		return v1 == v2 ? 1 : 0;
}

double LessThanCondition::distance(int v1, int v2, bool negated) const{
	if(!negated)
		return v1 < v2 ? 0 : v1 - v2 + 1;
	else
		return v1 >= v2 ? 0 : v2 - v1;
}

double LessThanOrEqualCondition::distance(int v1, int v2, bool negated) const{
	if(!negated)
		return v1 <= v2 ? 0 : v1 - v2;
	else
		return v1 > v2 ? 0 : v2 - v1 + 1;
}

double GreaterThanCondition::distance(int v1, int v2, bool negated) const{
	if(!negated)
		return v1 > v2 ? 0 : v2 - v1 + 1;
	else
		return v1 <= v2 ? 0 : v1 - v2;
}

double GreaterThanOrEqualCondition::distance(int v1, int v2, bool negated) const{
	if(!negated)
		return v1 >= v2 ? 0 : v2 - v1;
	else
		return v1 < v2 ? 0 : v1 - v2 + 1;
}


}}
