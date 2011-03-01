#include <sstream>
#include "CTLExprs.h"

using namespace std;

namespace PetriEngine { namespace CTL {

int PlaceExpr::evaluate(const Mark* marking){
	return _offset;
}

string PlaceExpr::toString() {
	return _name;
}

int LiteralExpr::evaluate(const Mark* marking){
	return _value;
}

string LiteralExpr::toString() {
	stringstream s;
	s << _value;
	return s.str();
}

int NotExpr::evaluate(const Mark* marking){
	if(child()->evaluate(marking) == 0)
		return 1;
	return 0;
}

string NotExpr::toString(){
	return "!" + child()->toString();
}

int AndExpr::evaluate(const Mark* marking){
	if(left()->evaluate(marking) != 0 && right()->evaluate(marking) != 0)
		return 1;
	return 0;
}

string AndExpr::toString() {
	return "("+left()->toString() + " AND " + right()->toString()+")";
}

int OrExpr::evaluate(const Mark* marking){
	if(left()->evaluate(marking) != 0 || right()->evaluate(marking) != 0)
		return 1;
	return 0;
}

string OrExpr::toString() {
	return "("+left()->toString() + " OR " + right()->toString()+")";
}

int EqualExpr::evaluate(const Mark* marking){
	if(left()->evaluate(marking) == right()->evaluate(marking))
		return 1;
	return 0;
}

string EqualExpr::toString() {
	return "("+left()->toString() + " == " + right()->toString()+")";
}

int NEqualExpr::evaluate(const Mark* marking){
	if(left()->evaluate(marking) != right()->evaluate(marking))
		return 1;
	return 0;
}

string NEqualExpr::toString() {
	return "("+left()->toString() + " != " + right()->toString()+")";
}

int LessExpr::evaluate(const Mark* marking){
	if(left()->evaluate(marking) < right()->evaluate(marking))
		return 1;
	return 0;
}

string LessExpr::toString() {
	return "("+left()->toString() + " < " + right()->toString()+")";
}

int LessEqualExpr::evaluate(const Mark* marking){
	if(left()->evaluate(marking) <= right()->evaluate(marking))
		return 1;
	return 0;
}

string LessEqualExpr::toString() {
	return "("+left()->toString() + " <= " + right()->toString()+")";
}

int GreaterExpr::evaluate(const Mark* marking){
	if(left()->evaluate(marking) > right()->evaluate(marking))
		return 1;
	return 0;
}

string GreaterExpr::toString() {
	return "("+left()->toString() + " > " + right()->toString()+")";
}

int GreaterEqualExpr::evaluate(const Mark* marking){
	if(left()->evaluate(marking) >= right()->evaluate(marking))
		return 1;
	return 0;
}

string GreaterEqualExpr::toString() {
	return "("+left()->toString() + " >= " + right()->toString()+")";
}

int PlusExpr::evaluate(const Mark* marking) {
	return left()->evaluate(marking) + right()->evaluate(marking);
}

string PlusExpr::toString() {
	return "("+left()->toString() + " + " + right()->toString()+")";
}

int MinusExpr::evaluate(const Mark* marking) {
	return left()->evaluate(marking) - right()->evaluate(marking); 
}

string MinusExpr::toString() {
	return "("+left()->toString() + " - " + right()->toString()+")";
}

int MultiplyExpr::evaluate(const Mark* marking) {
	return left()->evaluate(marking) * right()->evaluate(marking);
}

string MultiplyExpr::toString() {
	return "("+left()->toString() + " * " + right()->toString()+")";
}

int DivideExpr::evaluate(const Mark* marking) {
	return left()->evaluate(marking) / right()->evaluate(marking);
}

string DivideExpr::toString() {
	return "("+left()->toString() + " / " + right()->toString()+")";
}

}}
