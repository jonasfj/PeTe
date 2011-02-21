#include "petrinetfactory.h"

PetriNetFactory::PetriNetFactory() : AbstractPetriNetFactory(){
}

void PetriNetFactory::addPlace(const string &name){
	places.push_back(name);
}

void PetriNetFactory::addTransition(const string &name){
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

PetriNet PetriNetFactory::makePetriNet(){
	PetriNet net(places.size(), transitions.size());
	vector<Arc>::iterator it;
	for(it = inputArcs.begin(); it < inputArcs.end(); it++){
	}
	for(it = outputArcs.begin(); it < outputArcs.end(); it++){
	}
}
