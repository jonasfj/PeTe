#include "ExpressionClasses.h"

#include <assert.h>

/******************** Destructors ********************/

Expr::~Expr(){}

BinaryExpr::~BinaryExpr(){
	if(expr1)
		delete expr1;
	expr1 = NULL;
	if(expr2)
		delete expr2;
	expr2 = NULL;
}

MinusExpr::~MinusExpr(){
	if(_expr)
		delete _expr;
	_expr = NULL;
}

Condition::~Condition(){}

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

std::string BinaryExpr::toString() const{
	return "(" + expr1->toString() + " " + op() + " " + expr2->toString() + ")";
}

std::string MinusExpr::toString() const{
	return "-" + _expr->toString();
}

/******************** Context Analysis ********************/

void BinaryExpr::analyze(AnalysisContext& context){
	expr1->analyze(context);
	expr2->analyze(context);
}

void IdentiferExpr::analyze(AnalysisContext& context){
	AnalysisContext::ResolutionResult result = context.resolve(name);
	if(result.success){
		_offsetInMarking = result.offset;
		isPlace = result.isPlace;
	}
	else{
		ExprError error("Unable to resolve identifier \"" + name + "\"",
						_srcOffset,
						_name.length());
		context.reportError(error);
	}
}

/******************** Evaluation ********************/

int BinaryExpr::evaluate(const Marking marking, const Assignment variables){
	int v1 = _expr1->evaluate(marking, variables);
	int v2 = _expr2->evaluate(marking, variables);
	return apply(v1, v2);
}

int MinusExpr::evaluate(const Marking marking, const Assignment variables){
	return -(_expr->evaluate(marking, variables));
}

int LiteralExpr::evaluate(const Marking marking, const Assignment variables){
	return _value;
}

int IdentifierExpr::evaluate(const Marking marking, const Assignment variables){
	assert(_offsetInMarking != -1);
	if(isPlace)
		return GET_TOKENS(marking, _offsetInMarking);
	return GET_VALUE(variables, _offsetInMarking);
}

bool LogicalCondition::evaluate(const Marking marking, const Assignment variables){
	bool b1 = _cond1->evaluate(marking,variables);
	bool b2 = _cond2->evaluate(marking,variables);
	return apply(b1,b2);
}

bool CompareCondition::evaluate(const Marking marking, const Assignment variables){
	int v1 = _expr1->evaluate(marking,variables);
	int v2 = _expr2->evaluate(marking,variables);
	return apply(v1,v2);
}


bool NotCondition::evaluate(const Marking marking, const Assignment variables){
	return !(_cond->evaluate(marking, variables));
}

/******************** Apply (BinaryExpr subclasses) ********************/

int PlusExpr::apply(int v1, int v2){
	return v1 + v2;
}

int SubtractExpr::apply(int v1, int v2){
	return v1 - v2;
}

int MultiExpr::apply(int v1, int v2){
	return v1 * v2;
}

/******************** Apply (LogicalCondition subclasses) ********************/

bool AndCondition::apply(bool b1, bool b2){
	return b1 && b2;
}

bool OrCondition::apply(bool b1, bool b2){
	return b1 || b2;
}

/******************** Apply (CompareCondition subclasses) ********************/
bool EqualCondition::apply(int v1, int v2){
	return v1 == v2;
}

bool NotEqualCondition::apply(int v1, int v2){
	return v1 != v2;
}

bool LessThanCondition::apply(int v1, int v2){
	return v1 < v2;
}

bool LessThanOrEqualCondition::apply(int v1, int v2){
	return v1 <= v2;
}

bool GreaterThanCondition::apply(int v1, int v2){
	return v1 > v2;
}

bool GreaterThanOrEqualCondition::apply(int v1, int v2){
	return v1 >= v2;
}
/******************** Op (BinaryExpr subclasses) ********************/

std::string PlusExpr::op() const{
	return "+";
}

std::string SubtractExpr::op() const{
	return "-";
}

std::string MultiExpr::op() const{
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

/******************** Just-In-Time Compilation ********************/

