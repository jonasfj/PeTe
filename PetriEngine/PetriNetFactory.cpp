#include "PetriNetFactory.h"
#include "PetriNet.h"

using namespace std;

namespace PetriEngine{

PetriNetFactory::PetriNetFactory() : AbstractPetriNetFactory(){
}

void PetriNetFactory::addPlace(const string &name, int tokens, double, double){
	places.push_back(name);
	initialMarking.push_back(tokens);
}

void PetriNetFactory::addTransition(const string &name, double, double){
	transitions.push_back(name);
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
	PetriNet* net = new PetriNet(places.size(), transitions.size());
	size_t i;
	//Create place names
	for(i = 0; i < places.size(); i++)
		net->_placeNames[i] = places[i];
	//Create transition names
	for(i = 0; i < transitions.size(); i++)
		net->_transitionNames[i] = transitions[i];
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
			net->_transitions[transition * places.size() + place] = -arc->weight;
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
			net->_transitions[transition * places.size() + place] = arc->weight;
		}
	}
	//Return the finished net
	return net;
}

Mark* PetriNetFactory::makeInitialMarking(){
	Mark* mark = new Mark[places.size()];
	for(int i = 0; i < places.size(); i++)
		mark[i] = initialMarking[i];
	return mark;
}

} // PetriEngine
