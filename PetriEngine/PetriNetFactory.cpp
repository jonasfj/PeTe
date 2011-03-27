#include "PetriNetFactory.h"
#include "PetriNet.h"

#include "PQL/PQLParser.h"

using namespace std;

namespace PetriEngine{

PetriNetFactory::PetriNetFactory() : AbstractPetriNetFactory(){
}

void PetriNetFactory::addPlace(const string &name, int tokens, double, double){
	places.push_back(name);
	initialMarking.push_back(tokens);
}

void PetriNetFactory::addVariable(const string &name, int initialValue){
	variables.push_back(name);
	initialVariableValues.push_back(initialValue);
}

void PetriNetFactory::addTransition(const string &name,
									const string &condition,
									const string &assignment,
									double, double){
	transitions.push_back(name);
	conditions.push_back(condition);
	assignments.push_back(assignment);
}

void PetriNetFactory::addInputArc(const string &place, const string &transition, int weight){
	Arc arc;
	arc.place = place;
	arc.transition = transition;
	arc.weight = weight;
	inputArcs.push_back(arc);
}

void PetriNetFactory::addOutputArc(const string &transition, const string &place, int weight){
	Arc arc;
	arc.transition = transition;
	arc.place = place;
	arc.weight = weight;
	outputArcs.push_back(arc);
}

PetriNet* PetriNetFactory::makePetriNet(){
	PetriNet* net = new PetriNet(places.size(), transitions.size(), variables.size());
	size_t i;
	//Create variables
	for(i = 0; i < variables.size(); i++)
		net->_variables[i] = variables[i];
	//Create place names
	for(i = 0; i < places.size(); i++)
		net->_places[i] = places[i];
	//Create transition names
	for(i = 0; i < transitions.size(); i++)
		net->_transitions[i] = transitions[i];
	//Parse conditions and assignments
	for(i = 0; i < transitions.size(); i++){
		if(conditions[i] != "")
			net->_annotations[i].condition = PQL::ParseQuery(conditions[i]);
		if(assignments[i] != "")
			net->_annotations[i].assignment= PQL::ParseAssignment(assignments[i]);
		//TODO: Report parsing errors, if any
	}
	//Create input arcs
	vector<Arc>::iterator arc;
	for(arc = inputArcs.begin(); arc != inputArcs.end(); arc++){
		int place = -1, transition = -1;
		//Find place number
		for(i = 0; i < places.size(); i++){
			if(places[i] == arc->place){
				place = i;
				break;
			}
		}
		//Find transition number
		for(i = 0; i < transitions.size(); i++){
			if(transitions[i] == arc->transition){
				transition = i;
				break;
			}
		}
		//Abort if we couldn't create the arc
		if(place < 0 || transition < 0)
			continue;
		else{
			net->_transitionMatrix[transition * places.size() + place] = -arc->weight;
		}
	}
	//Create output arcs
	for(arc = outputArcs.begin(); arc != outputArcs.end(); arc++){
		int place = -1, transition = -1;
		//Find place number
		for(i = 0; i < places.size(); i++){
			if(places[i] == arc->place){
				place = i;
				break;
			}
		}
		//Find transition number
		for(i = 0; i < transitions.size(); i++){
			if(transitions[i] == arc->transition){
				transition = i;
				break;
			}
		}
		//Abort if we couldn't create the arc
		if(place < 0 || transition < 0)
			continue;
		else{
			net->_transitionMatrix[transition * places.size() + place] = arc->weight;
		}
	}
	//Return the finished net
	return net;
}

Marking PetriNetFactory::makeInitialMarking(){
	ALLOCATE_MARKING(mark,places.size());
	for(size_t i = 0; i < places.size(); i++)
		SET_TOKENS(mark,i,initialMarking[i]);
	return mark;
}
Assignment PetriNetFactory::makeInitialAssignment(){
	ALLOCATE_ASSIGNMENT(assign, variables.size());
	for(size_t i = 0; i < variables.size(); i++)
		SET_TOKENS(assign, i, initialVariableValues[i]);
	return assign;
}


} // PetriEngine
