#include "DTAPNTranslator.h"

#define MAX(a,b)	(a < b ? b : a)
#include <sstream>
#include <assert.h>

using namespace std;

/** Auxiliary function for converting from int to string */
std::string i2s(int i){
	stringstream ss;
	ss<<i;
	return ss.str();
}

namespace PetriEngine{

void DTAPNTranslator::addPlace(const std::string& name, int tokens){
	Place p;
	p.name = escapeIdentifier(name);
	p.tokens = tokens;
	p.maxAge = 0;
	places.push_back(p);
}

void DTAPNTranslator::addTransition(const std::string& name){
	Transition t;
	t.name = escapeIdentifier(name);
	transitions.push_back(t);
}

void DTAPNTranslator::addInArc(const std::string& place,
							   const std::string& transition,
							   int startInterval, int endInterval){
	InArc a;
	a.start = escapeIdentifier(place);
	a.end = escapeIdentifier(transition);
	a.startInterval = startInterval;
	a.endInterval = endInterval;
	inArcs.push_back(a);
}

void DTAPNTranslator::addOutArc(const std::string& transition, const std::string& place){
	OutArc a;
	a.start = escapeIdentifier(transition);
	a.end = escapeIdentifier(place);
	outArcs.push_back(a);
}

DTAPNTranslator::Place& DTAPNTranslator::findPlace(const string& name){
	for(PlaceIter p = places.begin(); p != places.end(); p++)
		if(p->name == name)
			return *p;
	// We should always find something
	assert(false);
	return places.front();
}

/** Make sure it can collide with stuff we have */
std::string DTAPNTranslator::escapeIdentifier(const std::string& identifier){
	//TODO: Test and improve this
	return identifier + "_";
}

/** Gets the input arcs of a transition */
DTAPNTranslator::InArcList DTAPNTranslator::preset(const std::string& transition){
	InArcList preset;
	for(InArcIter ai = inArcs.begin(); ai != inArcs.end(); ai++){
		if(ai->end == transition)
			preset.push_back(*ai);
	}
	return preset;
}

/** Gets the output arcs of a transition */
DTAPNTranslator::OutArcList DTAPNTranslator::postset(const std::string& transition){
	OutArcList postset;
	for(OutArcIter ai = outArcs.begin(); ai != outArcs.end(); ai++){
		if(ai->start == transition)
			postset.push_back(*ai);
	}
	return postset;
}

/** True, if place is the target of any output arcs */
bool DTAPNTranslator::isEndPlace(const std::string& place){
	for(OutArcIter ai = outArcs.begin(); ai != outArcs.end(); ai++){
		if(ai->end == place)
			return true;
	}
	return false;
}

void DTAPNTranslator::makePNDV(AbstractPetriNetBuilder* builder){
	this->builder = builder;

	// Find max age for all places
	for(InArcIter ai = inArcs.begin(); ai != inArcs.end(); ai++){
		Place& p = findPlace(ai->start);
		p.maxAge = MAX(p.maxAge, ai->endInterval);
	}

	// Find Largest postset
	int largestPostset = 0;
	for(TransitionIter t = transitions.begin(); t != transitions.end(); t++)
		largestPostset = MAX(largestPostset, postset(t->name).size());

	// Build control variables (Lock and Release Lock)
	lockStateIdle = transitions.size();
	lockStateAgeing = transitions.size() + 1;
	builder->addVariable("L", 0, transitions.size() + 1); //one for each transition, idle, ageing
	builder->addVariable("R", 0, largestPostset);

	// Build all original places
	for(PlaceIter p = places.begin(); p != places.end(); p++){
		builder->addPlace(p->name, p->tokens); // Ignore coordinates
		// Build variables for token ages
		for(int i = 0; i < bound; i++)
			builder->addVariable(p->name + "_" + i2s(i), 0, p->maxAge);
		// Build post-places
		if(isEndPlace(p->name)){
			string postplace = p->name + "_post";
			builder->addPlace(postplace, 0);
			// Add a transition for each variable that can store the token age
			for(int i = 0; i < bound; i++){
				string tokVar = p->name + "_" + i2s(i);
				string postptrans = postplace + "_t" + i2s(i);
				builder->addTransition(postptrans, p->name + " == " + i2s(i), "L := L-1; " + tokVar + " :=0;");
				builder->addInputArc(postplace, postptrans);
				builder->addOutputArc(postptrans, p->name);
			}
		}
	}

	for(TransitionIter t = transitions.begin(); t != transitions.end(); t++)
		buildTransition(*t);

}

/*
Variables:
	L				Lock
	R				Release lock
	<place>_<i>		Token i at <place>
Places:
	<place>						Original place
	<transition>_pre_<i>		Pre-place for transition
	<place>_post				Post-place for place, e.g. output place from transition to original place
Transitions:
	<transition>				Original transition
	<place>_post_t_<i>			Post-place transition for setting token i
*/

void DTAPNTranslator::buildTransition(Transition& t){
	InArcList  pre  = preset(t.name);
	OutArcList post = postset(t.name);
	// Add original transition, remember to set release lock
	builder->addTransition(t.name, "", "L := " + i2s(lockStateIdle) + "; R := " + i2s(post.size()) + ";");

	int inArcNr = 0;
	for(InArcIter ai = pre.begin(); ai != pre.end(); ai++){
		// Create pre-place
		string preplace = t.name+"_pre_"+i2s(inArcNr);
		builder->addPlace(preplace, 0);
		// Create k new k-transitions
		for(int i = 0; i < bound; i++){
			string pretrans = t.name+"_pre_t_"+i2s(inArcNr)+ "_"+i2s(i);

/*
precondition:
	preplace == 0
	ai->start >= i
	ai->startInterval <= <ai->start>_<i>
	<ai->start>_<i> <= ai->endInterval
	L == LockStateIdle or L == lockState(t.name)
	R == 0

post assignment:
	L := lockState(t.name)
	<ai->start>_<i>   := <ai->start>_<i+1>
	<ai->start>_<i+1> := <ai->start>_<i+2>
	...

*/
			// Set-up pre-conditions
			string preconds = "";
			for(int v = 0; v < bound; v++){

			}
			// Honor the locks
			preconds += " and R == 0 and L == "+i2s(lockStateIdle);

			string var = ai->start + "_pre_";

			// Grab one token and shift all variables larger than
			string postconds = "";
			for(int v = i; v < bound; v++){
				if(v != bound-1)
					postconds += var+i2s(v)+" := "+var+i2s(v+1)+";";
			}
			//TODO: Set lock to the transition index
			postconds += "";

			builder->addTransition(pretrans, preconds, postconds);

			// Add arc from original place to new transition
			builder->addInputArc(ai->start, pretrans);
			// Add arc from transition to pre-place.
			builder->addOutputArc(pretrans,preplace);
		}
		// Connect pre-place to original transition
		builder->addInputArc(preplace,t.name);
		inArcNr++;
	}

	// Create arcs to existing output arcs
	for(OutArcIter ai = post.begin(); ai != post.end(); ai++){
		string postplace = ai->end + "_post"; // Already exists
		builder->addOutputArc(t.name, postplace);
	}
}


} // PetriEngine
