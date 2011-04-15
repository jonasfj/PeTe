#include "PetriNetBuilder.h"
#include "PetriNet.h"

#include "PQL/PQLParser.h"
#include "PQL/PQL.h"
#include "PQL/CompiledCondition.h"

#include <assert.h>

using namespace std;

namespace PetriEngine{

PetriNetBuilder::PetriNetBuilder(bool JIT) : AbstractPetriNetBuilder(){
	_jit = JIT;
}

void PetriNetBuilder::addPlace(const string &name, int tokens, double, double){
	places.push_back(name);
	initialMarking.push_back(tokens);
}

void PetriNetBuilder::addVariable(const string &name, int initialValue, int range){
	variables.push_back(name);
	initialVariableValues.push_back(initialValue);
	ranges.push_back(range);
}

void PetriNetBuilder::addTransition(const string &name,
									const string &condition,
									const string &assignment,
									double, double){
	transitions.push_back(name);
	conditions.push_back(condition);
	assignments.push_back(assignment);
}

void PetriNetBuilder::addInputArc(const string &place, const string &transition, int weight){
	Arc arc;
	arc.place = place;
	arc.transition = transition;
	arc.weight = weight;
	inputArcs.push_back(arc);
}

void PetriNetBuilder::addOutputArc(const string &transition, const string &place, int weight){
	Arc arc;
	arc.transition = transition;
	arc.place = place;
	arc.weight = weight;
	outputArcs.push_back(arc);
}

PetriNet* PetriNetBuilder::makePetriNet(){
	PetriNet* net = new PetriNet(places.size(), transitions.size(), variables.size());
	size_t i;
	//Create variables
	for(i = 0; i < variables.size(); i++){
		net->_variables[i] = variables[i];
		net->_ranges[i] = ranges[i];
	}
	//Create place names
	for(i = 0; i < places.size(); i++)
		net->_places[i] = places[i];
	//Create transition names
	for(i = 0; i < transitions.size(); i++)
		net->_transitions[i] = transitions[i];
	//Parse conditions and assignments
	for(i = 0; i < transitions.size(); i++){
		if(conditions[i] != ""){
			net->_conditions[i] = PQL::ParseQuery(conditions[i]);
			if(net->_conditions[i]){
				PQL::AnalysisContext context(*net);
				if(_jit){
					PQL::CompiledCondition* CC = new PQL::CompiledCondition(net->_conditions[i]);
					CC->analyze(context);
					if(CC->compile())
						net->_conditions[i] = CC;
					else{
						delete CC;
						CC = NULL;
						//TODO: Print to stderr
					}
				}else
					net->_conditions[i]->analyze(context);

				//Delete if there we're errors
				if(context.errors().size() > 0){
					delete net->_conditions[i];
					net->_conditions[i] = NULL;
					//TODO: Print to stderr
				}
			}
		}
		if(assignments[i] != ""){
			net->_assignments[i] = PQL::ParseAssignment(assignments[i]);
			if(net->_assignments[i]){
				PQL::AnalysisContext context(*net);
				net->_assignments[i]->analyze(context);
				//Delete if there we're errors
				if(context.errors().size() > 0){
					delete net->_assignments[i];
					net->_assignments[i] = NULL;
					//TODO: Print to stderr
				}
			}
		}
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
		//We should have found a places and transition
		assert(place >= 0 && transition >= 0);
		net->_tv(transition)[place] = arc->weight;
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
		//We should have found a places and transition
		assert(place >= 0 && transition >= 0);
		net->_tv(transition)[place + places.size()] = arc->weight;
	}
	//Return the finished net
	return net;
}

MarkVal* PetriNetBuilder::makeInitialMarking(){
	MarkVal* m = new MarkVal[places.size()];
	for(size_t i = 0; i < places.size(); i++)
		m[i] = initialMarking[i];
	return m;
}
VarVal* PetriNetBuilder::makeInitialAssignment(){
	VarVal* a = new VarVal[variables.size()];
	for(size_t i = 0; i < variables.size(); i++)
		a[i] = initialVariableValues[i];
	return a;
}


} // PetriEngine
