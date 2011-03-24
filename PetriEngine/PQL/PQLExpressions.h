#include <assert.h>

/** Marking representation as an integer array.
  * Use DECLARE_MARKING macro to initialise the marking on the heap,
  * and ALLOCATE_MARKING to initialise on the stack 
  */
typedef Marking int*;
#define DECLARE_MARKING(m, s)		int m[s]
#define ALLOCATE_MARKING(m, s)		Marking m = (Marking)new int[s]
#define GET_TOKENS(m, o)			m[o]
#define SET_TOKENS(m, o, v)			m[o] = v

/** Assignment representation as an integer array.
  * Use DECLARE_ASSIGNMENT macro to initialise the assignment on the heap,
  * and ALLOCATE_ASSIGNMENT to initialise on the stack 
  */
typedef Assignment int*;
#define DECLARE_ASSIGNMENT(a, s)	int a[s]
#define ALLOCATE_ASSIGNMENT(a, s)	int Assignment a = (Assignment)new int[s]
#define GET_VALUE(a, o)				a[o]
#define SET_TOKENS(a, o, v)			a[o] = v


/** Representation of an error */
class ExprError{
	std::string _text;
	int _offset;
	int _length;
public:
	Error(std::string text, int offset = -1, int length = 0){
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
};

/** Context provided for context analysis */
class AnalysisContext{
private:
	const PetriNet& _net
	bool _usePlaces;
	std::list<ExprError> _errors;
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
	/** Create a context analysis context */
	AnalysisContext(const PetriNet& net, bool usePlaces) : _net(net) {
		_usePlaces = usePlaces;
	}
	/** Resolve an identifier */
	ResolutionResult resolve(std::string identifier) const{
		ResolutionResult result;
		result.offset = -1;
		if(_usePlaces){
			result.offset = _net.lookupPlace(identifier);
			result.isPlace = true;
			result.success = result.offset =! -1;
			if(result.success)
				return result;
		}
		result.offset = _net.lookupVariable(identifier);
		result.success = result.offset != -1;
		result.isPlace = false;
		return result;
	}
	/** Report error */
	void reportError(const ExprError& error){
		_errors.push_back(error);
	}
	/** Get list of errors */
	const std::list<ExprError>& errors() const{
		return _errors;
	}
};

/******************** EXPRESSIONS ********************/

/** Representation of an expression */
class Expr{
public:
	/** Virtual destructor, an expression should know it subexpressions */
	virtual ~Expr();
	/** Evaluate the expression given marking and assignment */
	virtual int evaluate(const Marking marking, const Assignment variables) const = 0;
	/** Perform context analysis */
	virtual void analyze(AnalysisContext& context) = 0;
	/** Convert expression to string */
	virtual std::string toString() const = 0;
};

/** Base class for all binary expressions */
class BinaryExpr : Expr{
public:
	BinaryExpr(Expr* expr1, Expr* expr2){
		_expr1 = expr1;
		_expr2 = expr2;
	}
	~BinaryExpr();
	void analyze(AnalysisContext& context);
	int evaluate(const Marking marking, const Assignment variables) const;
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
	SubtractExpr(Expr* expr1, Expr2* expr2) : BinaryExpr(expr1, expr2) {}
private:
	int apply(int v1, int v2) const;
	std::string op() const;
};

/** Binary minus expression */
class SubtractExpr : public BinaryExpr{
public: 
	SubtractExpr(Expr* expr1, Expr2* expr2) : BinaryExpr(expr1, expr2) {}
private:
	int apply(int v1, int v2) const;
	std::string op() const;
};

/** Binary multiplication expression **/
class MultExpr : public BinaryExpr{
public:
	MultExpr(Expr* expr1, Expr2* expr2) : BinaryExpr(expr1, expr1) {}
private:
	int apply(int v1, int v2) const;
	std:string op() const;
}

/** Unary minus expression*/
class MinusExpr : public Expr {
public:
	MinusExpr(Expr* expr){
		_expr = expr;
	}
	~MinusExpr();
	int evaluate(const Marking marking, const Assignment variables) const;
	void analyze(AnalysisContext& context);
	std::string toString() const;
private:
	Expr* _expr;
};

/** Literal integer value expression */
class LiteralExpr : public Expr {
public:
	LiteralExpr(int value) : _value(value){}
	int evaluate(const Marking marking, const Assignment variable) const;
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
	int evaluate(const Marking marking, const Assignment variable) const;
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

/* Base condition */
class Condition{
public:
	virtual ~Condition();
	virtual bool evaluate(const Marking marking, const Assignment variable) = 0;
	virtual void analyze(AnalysisContext& context) = 0;
	virtual std::string toString() = 0;
};

/* Logical conditon */
class LogicalCondition : public Condition{
public:
	LogicalCondition(Condition* cond1, Condition* cond2){
		_cond1 = cond1;
		_cond2 = cond2;
	}
	~LogicalCondition();
private:
	virtual bool apply(bool b1, bool b2) = 0;
	virtual std::string op() = 0;
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
private:
	virtual bool apply(int v1, int v2) = 0;
	virtual std::string op() = 0;
	Expr* _expr1;
	Expr* _expr2;
};

/* Equality conditon */
class EqualCondition : public CompareCondition{
public:
	EqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std:string op() const;
};

/* None equality conditon */
class NotEqualCondition : public CompareCondition{
public:
	NotEqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std:string op() const;
};

/* Less-than conditon */
class LessThanCondition : public CompareCondition{
public:
	LessThanCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std:string op() const;
};

/* Less-than-or-equal conditon */
class LessThanOrEqualCondition : public CompareCondition{
public:
	LessThanOrEqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std:string op() const;
};

/* Greater-than conditon */
class GreaterThanCondition : public CompareCondition{
public:
	GreaterThanCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std:string op() const;
};

/* Greater-than-or-equal conditon */
class GreaterThanOrEqualCondition : public CompareCondition{
public:
	GreaterThanOrEqualCondition(Expr* expr1, Expr* expr2) : CompareCondition(expr1,expr2) {}
private:
	bool apply(int v1, int v2) const;
	std:string op() const;
};

/* Not condition */
class NotCondition : public Condition{
public:
	NotCondition(Condition* cond){
		_cond = cond;
	}
	~NotCondition();
	bool evaluate(const Marking marking, const Assignment variable);
	void analyze(AnalysisContext& context);
	std::string toString();
private:
	Cond* _cond;
};


/******************** Assignment ********************/

/**  */
class AssignmentExpression{
private:
	struct VariableAssignment{
		std::string identifier;
		int offset;
		Expr* expr;
	};
	typedef std::list<VariableAssignment>::iterator iter;
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
			if(result.success){
			}else{
				ExprError error(...);
				context.reportError(error);
			}
		}
	}
private:
	std::list<VariableAssignment> assignments;
};
