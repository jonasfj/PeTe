#include "PetriNet.h"
#include "PQL/PQLExpressions.h"
#include <stdio.h>
#include <string.h>
using namespace std;

namespace PetriEngine{

PetriEngine::PetriNet::PetriNet(int places, int transitions, int variables){
	//Store size for later
	_nPlaces = places;
	_nTransitions = transitions;
	_nVariables = variables;

	//Allocate space for names
	_places = new std::string[places];
	_transitions = new std::string[transitions];
	_variables = new std::string[variables];

	//Allocate space for conditions and assignments
	_annotations = new AnnotationPair[transitions];
	for(int i = 0; i < transitions; i++){
		this->_annotations[i].condition = NULL;
		this->_annotations[i].assignment = NULL;
	}

	//Allocate transition matrix
	_transitionMatrix = new Mark[places*transitions];
	//Set transition matrix to zero
	for(int i = 0; i < places*transitions; i++)
		_transitionMatrix[i] = 0;
}

bool PetriEngine::PetriNet::fire(unsigned int transition,
								 const Marking marking,
								 const Assignment assignment,
								 Marking resultMarking,
								 Assignment resultAssignment) const{
	//Check the condition
	if(_annotations[transition].condition &&
	   !_annotations[transition].condition->evaluate(NULL, assignment))
		return false;

	Mark* t = _transitionMatrix + transition * _nPlaces;
	for(int i = 0; i < _nPlaces; i++){
		resultMarking[i] = marking[i] + t[i];
		if(resultMarking[i] < 0)
			return false;
	}

	if(_annotations[transition].assignment)
		_annotations[transition].assignment->evaluate(assignment, resultAssignment);
	else
		memcpy(resultAssignment, assignment, sizeof(Mark)*this->_nVariables);

	return true;
}

int PetriNet::lookupPlace(const string &name) const{
	for(int i = 0; i < _nPlaces; i++){
		if(_places[i] == name)
			return i;
	}
	return -1;
}

int PetriNet::lookupVariable(const string &name) const{
	for(int i = 0; i < _nVariables; i++){
		if(_variables[i] == name)
			return i;
	}
	return -1;
}

} // PetriEngine
