#include "PetriNet.h"
#include "PQL/PQL.h"
#include "PQL/Contexts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

namespace PetriEngine{

PetriNet::PetriNet(int places, int transitions, int variables)
	: _places(places), _transitions(transitions), _variables(variables) {
	//Store size for later
	_nPlaces = places;
	_nTransitions = transitions;
	_nVariables = variables;

	//Allocate space for ranges
	_ranges = new VarVal[variables];

	//Allocate space for conditions and assignments
	_conditions = (PQL::Condition**)calloc(sizeof(PQL::Condition*) + sizeof(PQL::AssignmentExpression*),
										  transitions);
	_assignments = (PQL::AssignmentExpression**)(_conditions + transitions);

	//Allocate transition matrix
	_tm = (MarkVal*)calloc(sizeof(MarkVal), places * transitions * 2);
}

PetriNet::~PetriNet(){
	if(_ranges)
		delete _ranges;
	_ranges = NULL;
	if(_tm)
		free(_tm);
	_tm = NULL;
	//Conditions and assignments is allocated in the same block
	if(_conditions)
		free(_conditions);
	_conditions = NULL;
	_assignments = NULL;
}

bool PetriNet::fire(unsigned int t,
					const MarkVal* m,
					const VarVal* a,
					MarkVal* result_m,
					VarVal* result_a) const{
	//Check the condition
	if(_conditions[t] &&
	   !_conditions[t]->evaluate(PQL::EvaluationContext(m, a)))
		return false;

	const MarkVal* tv = _tv(t);
	//Check that we can take from the marking
	for(size_t i = 0; i < _nPlaces; i++){
		result_m[i] = m[i] - tv[i];
		if(result_m[i] < 0)
			return false;
	}
	//Add stuff that the marking gives us
	for(size_t i = 0; i < _nPlaces; i++)
		result_m[i] += tv[i+_nPlaces];


	if(_assignments[t])
		_assignments[t]->evaluate(m, a, result_a, _ranges, _nVariables);
	else
		memcpy(result_a, a, sizeof(VarVal) * _nVariables);

	return true;
}

bool PetriNet::fireWithMarkInf(unsigned int t,
							   const MarkVal* m,
							   const VarVal* a,
							   MarkVal* result_m,
							   VarVal* result_a) const{
	//Check the condition
	if(_conditions[t] && //TODO: Use evaluate that respects MarkInf
	   !_conditions[t]->evaluate(PQL::EvaluationContext(m, a)))
		return false;

	const MarkVal* tv = _tv(t);
	//Check that we can take from the marking
	for(size_t i = 0; i < _nPlaces; i++){
		if(m[i] == MARK_INF)
			continue;
		result_m[i] = m[i] - tv[i];
		if(result_m[i] < 0)
			return false;
	}
	//Add stuff that the marking gives us
	for(size_t i = 0; i < _nPlaces; i++){
		if(m[i] == MARK_INF)
			continue;
		result_m[i] += tv[i+_nPlaces];
	}


	if(_assignments[t]) //TODO: Use evaluate that respects MarkInf
		_assignments[t]->evaluate(m, a, result_a, _ranges, _nVariables);
	else
		memcpy(result_a, a, sizeof(VarVal) * _nVariables);

	return true;
}

int PetriNet::inArc(unsigned int place, unsigned int transition) const{
	return _tv(transition)[place];
}

int PetriNet::outArc(unsigned int transition, unsigned int place) const{
	return _tv(transition)[place + _nPlaces];
}

} // PetriEngine
