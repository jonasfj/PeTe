#ifndef CTLNODES_H
#define CTLNODES_H

#include <string>
#include "../PetriNet.h"

namespace PetriEngine{ namespace CTL {
class CTLExpr {
public: 
	virtual int evaluate(const Mark* marking) = 0;
	virtual void lookupOffset(PetriNet* network) = 0;
	virtual std::string toString() = 0;
};

class PlaceExpr: public CTLExpr {
public:
	PlaceExpr(const std::string& name): _name(name) {
		_offset = 0;
	}
	int evaluate(const Mark* marking);
	void lookupOffset(PetriNet* network);
	std::string toString();
private:
	std::string _name;
	int _offset;
};

class LiteralExpr: public CTLExpr {
public:
	LiteralExpr(int value): _value(value) {}
	int evaluate(const Mark* marking);
	void lookupOffset(PetriNet* network);
	std::string toString();
private:
	int _value;
};

class NotExpr: public CTLExpr {
public:
	NotExpr(CTLExpr* child){
		_child = child;
	}
	CTLExpr* child() {
		return _child;
	}
	int evaluate(const Mark* marking);
	void lookupOffset(PetriNet* network);
	std::string toString();
private:
	CTLExpr* _child;
};

class BinaryExpr: public CTLExpr {
public:
	CTLExpr* left() {
		return _lhand;
	}
	CTLExpr* right() {
		return _rhand;
	}
	virtual int evaluate(const Mark* marking) = 0;
	virtual std::string toString() = 0;
	void lookupOffset(PetriNet* network);
protected:
	BinaryExpr(CTLExpr* lhand, CTLExpr* rhand) {
		_lhand = lhand;
		_rhand = rhand;
	}
private:
	CTLExpr* _lhand;
	CTLExpr* _rhand;
};

class AndExpr: public BinaryExpr {
public:
	AndExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class OrExpr: public BinaryExpr {
public:
	OrExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class EqualExpr: public BinaryExpr {
public:
	EqualExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class NEqualExpr: public BinaryExpr {
public:
	NEqualExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class LessExpr: public BinaryExpr {
public:
	LessExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class LessEqualExpr: public BinaryExpr {
public:
	LessEqualExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class GreaterExpr: public BinaryExpr {
public:
	GreaterExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class GreaterEqualExpr: public BinaryExpr {
public:
	GreaterEqualExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class PlusExpr: public BinaryExpr {
public:
	PlusExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class MinusExpr: public BinaryExpr {
public:
	MinusExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class MultiplyExpr: public BinaryExpr {
public:
	MultiplyExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

class DivideExpr: public BinaryExpr {
public:
	DivideExpr(CTLExpr* l, CTLExpr* r): BinaryExpr(l,r) {}
	int evaluate(const Mark* marking);
	std::string toString();
};

}}

#endif
