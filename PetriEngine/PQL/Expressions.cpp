#include "Contexts.h"
#include "Expressions.h"

#include <sstream>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <set>

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
	return "(not " + _cond->toString() + ")";
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
		result_a[it->offset] = it->expr->evaluate(context) % (ranges[it->offset]+1);
}

/******************** Apply (BinaryExpr subclasses) ********************/

int PlusExpr::apply(int v1, int v2) const		{ return v1 + v2; }
int SubtractExpr::apply(int v1, int v2) const	{ return v1 - v2; }
int MultiplyExpr::apply(int v1, int v2) const	{ return v1 * v2; }

/******************** Apply (LogicalCondition subclasses) ********************/

bool AndCondition::apply(bool b1, bool b2) const	{ return b1 && b2; }
bool OrCondition::apply(bool b1, bool b2) const		{ return b1 || b2; }

/******************** Apply (CompareCondition subclasses) ********************/

bool EqualCondition::apply(int v1, int v2) const				{ return v1 == v2;	}
bool NotEqualCondition::apply(int v1, int v2) const				{ return v1 != v2;	}
bool LessThanCondition::apply(int v1, int v2) const				{ return v1 < v2;	}
bool LessThanOrEqualCondition::apply(int v1, int v2) const		{ return v1 <= v2;	}
bool GreaterThanCondition::apply(int v1, int v2) const			{ return v1 > v2;	}
bool GreaterThanOrEqualCondition::apply(int v1, int v2) const	{ return v1 >= v2;	}

/******************** Op (BinaryExpr subclasses) ********************/

std::string PlusExpr::op() const		{ return "+"; }
std::string SubtractExpr::op() const	{ return "-"; }
std::string MultiplyExpr::op() const	{ return "*"; }

/******************** Op (LogicalCondition subclasses) ********************/

std::string AndCondition::op() const	{ return "and";	}
std::string OrCondition::op() const		{ return "or";	}

/******************** Op (CompareCondition subclasses) ********************/

std::string EqualCondition::op() const				{ return "==";	}
std::string NotEqualCondition::op() const			{ return "!=";	}
std::string LessThanCondition::op() const			{ return "<";	}
std::string LessThanOrEqualCondition::op() const	{ return "<=";	}
std::string GreaterThanCondition::op() const		{ return ">";	}
std::string GreaterThanOrEqualCondition::op() const	{ return ">=";	}

/******************** p-free Expression ********************/

bool BinaryExpr::pfree() const		{ return _expr1->pfree() && _expr2->pfree(); }
bool MinusExpr::pfree() const		{ return _expr->pfree(); }
bool LiteralExpr::pfree() const		{ return true; }
bool IdentifierExpr::pfree() const	{ return !this->isPlace; }

/******************** Expr::type() implementation ********************/

Expr::Types PlusExpr::type() const			{ return Expr::PlusExpr;		}
Expr::Types SubtractExpr::type() const		{ return Expr::SubtractExpr;	}
Expr::Types MultiplyExpr::type() const		{ return Expr::MinusExpr;		}
Expr::Types MinusExpr::type() const			{ return Expr::MinusExpr;		}
Expr::Types LiteralExpr::type() const		{ return Expr::LiteralExpr;		}
Expr::Types IdentifierExpr::type() const	{ return Expr::IdentifierExpr;	}

/******************** Constraint Analysis ********************/

void LogicalCondition::findConstraints(ConstraintAnalysisContext& context) const{
	if(!context.canAnalyze)
		return;
	_cond1->findConstraints(context);
	ConstraintAnalysisContext::ConstraintSet left = context.retval;
	_cond2->findConstraints(context);
	mergeConstraints(context.retval, left, context.negated);
}

void AndCondition::mergeConstraints(ConstraintAnalysisContext::ConstraintSet& result,
									ConstraintAnalysisContext::ConstraintSet& other,
									bool negated) const{
	if(!negated)
		result = Structures::StateConstraints::mergeAnd(result, other);
	else
		result = Structures::StateConstraints::mergeOr(result, other);
}
void OrCondition::mergeConstraints(ConstraintAnalysisContext::ConstraintSet& result,
								   ConstraintAnalysisContext::ConstraintSet& other,
								   bool negated) const{
	if(!negated)
		result = Structures::StateConstraints::mergeOr(result, other);
	else
		result = Structures::StateConstraints::mergeAnd(result, other);
}

void CompareCondition::findConstraints(ConstraintAnalysisContext& context) const{
	if(!context.canAnalyze)
		return;
	context.retval.clear();
	if(_expr1->type() == Expr::LiteralExpr && _expr2->type() == Expr::IdentifierExpr){
		IdentifierExpr* id = (IdentifierExpr*)_expr2;
		LiteralExpr* literal = (LiteralExpr*)_expr1;
		addConstraints(context, literal->value(), id);
	}else if(_expr1->type() == Expr::IdentifierExpr && _expr2->type() == Expr::LiteralExpr){
		IdentifierExpr* id = (IdentifierExpr*)_expr1;
		LiteralExpr* literal = (LiteralExpr*)_expr2;
		addConstraints(context, id, literal->value());
	}else
		context.canAnalyze = false;
}

void NotCondition::findConstraints(ConstraintAnalysisContext& context) const{
	if(context.canAnalyze){
		context.negated = !context.negated;
		this->_cond->findConstraints(context);
		context.negated = !context.negated;
	}
}

/******************** CompareCondition::addConstraints ********************/


void EqualCondition::addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const{
	if(!context.negated){
		Structures::StateConstraints* s = NULL;
		if(id->pfree())
			s = Structures::StateConstraints::requireVarEqual(context.net(), id->offset(), value);
		else
			s = Structures::StateConstraints::requirePlaceEqual(context.net(), id->offset(), value);
		assert(s);
		context.retval.push_back(s);
	}else{
		Structures::StateConstraints* s1 = NULL;
		Structures::StateConstraints* s2 = NULL;
		if(id->pfree()){
			s1 = Structures::StateConstraints::requireVarMin(context.net(), id->offset(), value + 1);
			s2 = Structures::StateConstraints::requireVarMax(context.net(), id->offset(), value - 1);
		}else{
			s1 = Structures::StateConstraints::requirePlaceMin(context.net(), id->offset(), value + 1);
			s2 = Structures::StateConstraints::requirePlaceMax(context.net(), id->offset(), value - 1);
		}
		assert(s1 && s2);
		context.retval.push_back(s1);
		context.retval.push_back(s2);
	}
}

void EqualCondition::addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const{
	addConstraints(context, id, value);
}

void NotEqualCondition::addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const{
	if(context.negated){
		Structures::StateConstraints* s = NULL;
		if(id->pfree())
			s = Structures::StateConstraints::requireVarEqual(context.net(), id->offset(), value);
		else
			s = Structures::StateConstraints::requirePlaceEqual(context.net(), id->offset(), value);
		assert(s);
		context.retval.push_back(s);
	}else{
		Structures::StateConstraints* s1 = NULL;
		Structures::StateConstraints* s2 = NULL;
		if(id->pfree()){
			s1 = Structures::StateConstraints::requireVarMin(context.net(), id->offset(), value + 1);
			s2 = Structures::StateConstraints::requireVarMax(context.net(), id->offset(), value - 1);
		}else{
			s1 = Structures::StateConstraints::requirePlaceMin(context.net(), id->offset(), value + 1);
			s2 = Structures::StateConstraints::requirePlaceMax(context.net(), id->offset(), value - 1);
		}
		assert(s1 && s2);
		context.retval.push_back(s1);
		context.retval.push_back(s2);
	}
}

void NotEqualCondition::addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const{
	addConstraints(context, id, value);
}

void LessThanCondition::addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const{
	Structures::StateConstraints* nc = new Structures::StateConstraints(context.net());
	if(!context.negated){
		if(!id->pfree())
			nc->setPlaceMax(id->offset(), value-1);
		else
			nc->setVarMax(id->offset(), value-1);
	}else{
		if(!id->pfree())
			nc->setPlaceMin(id->offset(), value);
		else
			nc->setVarMin(id->offset(), value);
	}
	context.retval.push_back(nc);
}

void LessThanCondition::addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const{
	Structures::StateConstraints* nc = new Structures::StateConstraints(context.net());
	if(!context.negated){
		if(!id->pfree())
			nc->setPlaceMin(id->offset(), value+1);
		else
			nc->setVarMin(id->offset(), value+1);
	}else{
		if(!id->pfree())
			nc->setPlaceMax(id->offset(), value);
		else
			nc->setVarMax(id->offset(), value);
	}
	context.retval.push_back(nc);
}


void LessThanOrEqualCondition::addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const{
	Structures::StateConstraints* nc = new Structures::StateConstraints(context.net());
	if(!context.negated){
		if(!id->pfree())
			nc->setPlaceMax(id->offset(), value);
		else
			nc->setVarMax(id->offset(), value);
	}else{
		if(!id->pfree())
			nc->setPlaceMin(id->offset(), value+1);
		else
			nc->setVarMin(id->offset(), value+1);
	}
	context.retval.push_back(nc);
}

void LessThanOrEqualCondition::addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const{
	Structures::StateConstraints* nc = new Structures::StateConstraints(context.net());
	if(!context.negated){
		if(!id->pfree())
			nc->setPlaceMin(id->offset(), value);
		else
			nc->setVarMin(id->offset(), value);
	}else{
		if(!id->pfree())
			nc->setPlaceMax(id->offset(), value-1);
		else
			nc->setVarMax(id->offset(), value-1);
	}
	context.retval.push_back(nc);
}

void GreaterThanCondition::addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const{
	Structures::StateConstraints* nc = new Structures::StateConstraints(context.net());
	if(!context.negated){
		if(!id->pfree()) // p1 > 5
			nc->setPlaceMin(id->offset(), value+1);
		else
			nc->setVarMin(id->offset(), value+1);
	}else{
		if(!id->pfree())
			nc->setPlaceMax(id->offset(), value);
		else
			nc->setVarMax(id->offset(), value);
	}
	context.retval.push_back(nc);
}

void GreaterThanCondition::addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const{
	Structures::StateConstraints* nc = new Structures::StateConstraints(context.net());
	if(!context.negated){
		if(!id->pfree()) // 5 > p1
			nc->setPlaceMax(id->offset(), value-1);
		else
			nc->setVarMax(id->offset(), value-1);
	}else{
		if(!id->pfree()) // !(5 > p1)
			nc->setPlaceMin(id->offset(), value);
		else
			nc->setVarMin(id->offset(), value);
	}
	context.retval.push_back(nc);
}

void GreaterThanOrEqualCondition::addConstraints(ConstraintAnalysisContext& context,	IdentifierExpr* id, int value) const{
	Structures::StateConstraints* nc = new Structures::StateConstraints(context.net());
	if(!context.negated){
		if(!id->pfree()) // p1 >= 5
			nc->setPlaceMin(id->offset(), value);
		else
			nc->setVarMin(id->offset(), value);
	}else{
		if(!id->pfree()) // !(p1 >= 5)
			nc->setPlaceMax(id->offset(), value-1);
		else
			nc->setVarMax(id->offset(), value-1);
	}
	context.retval.push_back(nc);
}

void GreaterThanOrEqualCondition::addConstraints(ConstraintAnalysisContext& context, int value,	IdentifierExpr* id) const{
	Structures::StateConstraints* nc = new Structures::StateConstraints(context.net());
	if(!context.negated){
		if(!id->pfree()) // 5 >= p1
			nc->setPlaceMax(id->offset(), value);
		else
			nc->setVarMax(id->offset(), value);
	}else{
		if(!id->pfree()) // !(5 >= p1)
			nc->setPlaceMin(id->offset(), value+1);
		else
			nc->setVarMin(id->offset(), value+1);
	}
	context.retval.push_back(nc);
}

/******************** Distance Condition ********************/

#define MAX(v1, v2)		(v1 > v2 ? v1 : v2)
#define MIN(v1, v2)		(v1 > v2 ? v1 : v2)

double NotCondition::distance(DistanceContext& context) const{
	context.negate();
	double retval = _cond->distance(context);
	context.negate();
	return retval;
}

double LogicalCondition::distance(DistanceContext& context) const{
	double d1 = _cond1->distance(context);
	double d2 = _cond2->distance(context);
	return delta(d1, d2, context);
}

double AndCondition::delta(double d1,
						   double d2,
						   const DistanceContext& context) const{
	if(context.strategy() & DistanceContext::AndExtreme)
		if(context.negated())
			return MIN(d1, d2);
		else
			return MAX(d1, d2);
	else if(context.strategy() & DistanceContext::AndSum)
		return d1 + d2;
	else
		return (d1 + d2) / 2;
}

double OrCondition::delta(double d1,
						  double d2,
						  const DistanceContext& context) const{
	if(context.strategy() & DistanceContext::OrExtreme)
		if(context.negated())
			return MAX(d1, d2);
		else
			return MIN(d1, d2);
	else
		return (d1 + d2) / 2;
}

struct S{
	int d;
	unsigned int p;
};

int dfsArcLen(const PetriNet& net,
			  const MarkVal *m,
			  unsigned int place){
	std::list<S> places;
	std::set<unsigned int> visited;
	S s;
	s.d = 0;
	s.p = place;
	places.push_back(s);
	visited.insert(place);
	while(!places.empty()){
		s = places.front();
		places.pop_front();
		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			if(net.outArc(t, place)){
				for(unsigned int p = 0; p < net.numberOfPlaces(); p++){
					if(net.inArc(p, t)){
						if(!visited.count(p)){
							if(m[p])
								return s.d + 1;
							visited.insert(p);
							S sp;
							sp.d = s.d + 1;
							sp.p = p;
							places.push_back(sp);
						}
					}
				}
			}
		}
	}
	return s.d + 1;
}

double CompareCondition::distance(DistanceContext& context) const{
	int v1 = _expr1->evaluate(context);
	int v2 = _expr2->evaluate(context);
	if(context.strategy() & DistanceContext::ArcCount){
		int d = delta(v1, v2, context.negated());
		if(!d) return 0;
		if(_expr1->pfree() && !_expr2->pfree() && _expr2->type() == Expr::IdentifierExpr){
			IdentifierExpr* id = (IdentifierExpr*)_expr2;
			return dfsArcLen(context.net(), context.marking(), id->offset()) * d;
		}else if(_expr2->pfree() && !_expr1->pfree() && _expr1->type() == Expr::IdentifierExpr){
			IdentifierExpr* id = (IdentifierExpr*)_expr1;
			return dfsArcLen(context.net(), context.marking(), id->offset()) * d;
		}
	} else if(context.strategy() & DistanceContext::TokenCost){
		int d = delta(v1, v2, context.negated());
		if(d == 0) return 0;
		//TODO: Account for when we have too many tokens instead of too few
		if(_expr1->pfree() && !_expr2->pfree() && _expr2->type() == Expr::IdentifierExpr){
			IdentifierExpr* id = (IdentifierExpr*)_expr2;
			return context.distanceMatrix()->tokenCost(id->offset(), d, context.marking());
		}else if(_expr2->pfree() && !_expr1->pfree() && _expr1->type() == Expr::IdentifierExpr){
			IdentifierExpr* id = (IdentifierExpr*)_expr1;
			return context.distanceMatrix()->tokenCost(id->offset(), d, context.marking());
		}
	}
	return delta(v1, v2, context.negated());
}

double EqualCondition::delta(int v1, int v2, bool negated) const{
	if(!negated)
		return v1 > v2 ? v1 - v2 : v2 - v1;
	else
		return v1 == v2 ? 1 : 0;
}

double NotEqualCondition::delta(int v1, int v2, bool negated) const{
	if(negated)
		return v1 > v2 ? v1 - v2 : v2 - v1;
	else
		return v1 == v2 ? 1 : 0;
}

double LessThanCondition::delta(int v1, int v2, bool negated) const{
	if(!negated)
		return v1 < v2 ? 0 : v1 - v2 + 1;
	else
		return v1 >= v2 ? 0 : v2 - v1;
}

double LessThanOrEqualCondition::delta(int v1, int v2, bool negated) const{
	if(!negated)
		return v1 <= v2 ? 0 : v1 - v2;
	else
		return v1 > v2 ? 0 : v2 - v1 + 1;
}

double GreaterThanCondition::delta(int v1, int v2, bool negated) const{
	if(!negated)
		return v1 > v2 ? 0 : v2 - v1 + 1;
	else
		return v1 <= v2 ? 0 : v1 - v2;
}

double GreaterThanOrEqualCondition::delta(int v1, int v2, bool negated) const{
	if(!negated)
		return v1 >= v2 ? 0 : v2 - v1;
	else
		return v1 < v2 ? 0 : v1 - v2 + 1;
}


/******************** Just-In-Time Compilation ********************/

} // PQL
} // PetriEngine

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Function.h>
#include <llvm/CallingConv.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>
#include <llvm/InlineAsm.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/Transforms/Scalar.h>

using namespace llvm;

namespace PetriEngine{
namespace PQL{

/******************** Code Generation Expressions ********************/

Value* BinaryExpr::codegen(CodeGenerationContext &context) const{
	Value* v1 = _expr1->codegen(context);
	Value* v2 = _expr2->codegen(context);
	return BinaryOperator::Create((Instruction::BinaryOps)this->binaryOp(),
								  v1,
								  v2,
								  this->toString().c_str(),
								  context.label());
}

//BinaryExpr::binaryOp implementations
int PlusExpr::binaryOp() const						{ return Instruction::Add; }
int SubtractExpr::binaryOp() const					{ return Instruction::Sub; }
int MultiplyExpr::binaryOp() const					{ return Instruction::Mul; }


Value* MinusExpr::codegen(CodeGenerationContext &context) const{
	Value* value = _expr->codegen(context);
	Value* zero = ConstantInt::get(IntegerType::get(context.context(), 32),
								   0,
								   false);
	return BinaryOperator::Create(Instruction::Sub,
								  zero,
								  value,
								  this->toString().c_str(),
								  context.label());
}

Value* LiteralExpr::codegen(CodeGenerationContext &context) const{
	return ConstantInt::get(IntegerType::get(context.context(), 32),
							this->_value,
							true);
}

Value* IdentifierExpr::codegen(CodeGenerationContext &context) const{
	ConstantInt* literalOffset = ConstantInt::get(IntegerType::get(context.context(), 32),
												  this->_offsetInMarking,
												  false);
	GetElementPtrInst* deref;
	if(this->isPlace){
		deref = GetElementPtrInst::Create(context.marking(),
										  literalOffset,
										  "Deref place" + this->_name,
										  context.label());
	}else{
		deref = GetElementPtrInst::Create(context.valuation(),
										  literalOffset,
										  "Deref variable " + this->_name,
										  context.label());
	}
	return new LoadInst(deref, "Load " + this->_name, false, context.label());
}


/******************** Code Generation Conditions ********************/

Value* LogicalCondition::codegen(CodeGenerationContext &context) const{
	Value* v1 = _cond1->codegen(context);
	Value* v2 = _cond2->codegen(context);
	//TODO: Consider using branching...
	return BinaryOperator::Create((Instruction::BinaryOps)logicalOp(),
								  v1,
								  v2,
								  this->toString().c_str(),
								  context.label());
}

//LogicalCondition::logicalOp implementations
int AndCondition::logicalOp() const					{ return Instruction::And; }
int OrCondition::logicalOp() const					{ return Instruction::Or;  }

Value* CompareCondition::codegen(CodeGenerationContext& context) const{
	Value* v1 = _expr1->codegen(context);
	Value* v2 = _expr2->codegen(context);
	return new ICmpInst(*context.label(),
						(ICmpInst::Predicate)compareOp(),
						v1,
						v2,
						this->toString().c_str());
}

//CompareCondition::compareOp implementations
int EqualCondition::compareOp() const				{ return ICmpInst::ICMP_EQ;  }
int NotEqualCondition::compareOp() const			{ return ICmpInst::ICMP_NE;  }
int LessThanCondition::compareOp() const			{ return ICmpInst::ICMP_SLT; }
int LessThanOrEqualCondition::compareOp() const		{ return ICmpInst::ICMP_SLE; }
int GreaterThanCondition::compareOp() const			{ return ICmpInst::ICMP_SGT; }
int GreaterThanOrEqualCondition::compareOp() const	{ return ICmpInst::ICMP_SGE; }

Value* NotCondition::codegen(CodeGenerationContext& context) const{
	Value* value = _cond->codegen(context);
	Value* zero = ConstantInt::get(IntegerType::get(context.context(), 1),
								   0,
								   false);
	return new ICmpInst(ICmpInst::ICMP_EQ,
						value,
						zero,
						"Not operation");
}

} // PQL
} // PetriEngine

