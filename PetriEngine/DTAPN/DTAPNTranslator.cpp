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
namespace DTAPN {

void DTAPNTranslator::addPlace(const std::string& name, int tokens, double, double){
	Place p;
	p.name = escapeIdentifier(name);
	p.tokens = tokens;
	p.maxAge = 0;
	places.push_back(p);
}

void DTAPNTranslator::addTransition(const std::string& name, double, double){
	Transition t;
	t.name = escapeIdentifier(name);
	transitions.push_back(t);
}

void DTAPNTranslator::addInputArc(const std::string& place, const std::string& transition, int startInterval, int endInterval){
	InArc a;
	a.start = escapeIdentifier(place);
	a.end = escapeIdentifier(transition);
	a.startInterval = startInterval;
	a.endInterval = endInterval;
	inArcs.push_back(a);
}

void DTAPNTranslator::addOutputArc(const std::string& transition, const std::string& place){
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

/** Make sure it can't collide with stuff we have */
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

/** Translates a bounded DTAPN into a PNDV */
void DTAPNTranslator::makePNDV(AbstractPetriNetBuilder* builder){
	// Find max age for all places
	for(InArcIter ai = inArcs.begin(); ai != inArcs.end(); ai++){
		Place& p = findPlace(ai->start);
		p.maxAge = MAX(p.maxAge, ai->endInterval + 1);
	}

	// Replace infinity with max age
	for(InArcIter ai = inArcs.begin(); ai != inArcs.end(); ai++){
		Place& p = findPlace(ai->start);
		if(ai->endInterval == -1)
			ai->endInterval = p.maxAge;
	}

	// Find Largest postset
	int largestPostset = 0;
	for(TransitionIter t = transitions.begin(); t != transitions.end(); t++)
		largestPostset = MAX(largestPostset, postset(t->name).size());

	// Build control variables (Transition Lock and Release Lock)
	lockStateIdle = 0;
	lockStateAgeing = transitions.size();
	builder->addVariable("L", 0, transitions.size() + 1); //one for each transition + idle and ageing
	builder->addVariable("R", 0, largestPostset);

	// For each place
	for(PlaceIter p = places.begin(); p != places.end(); p++){
		// Build original place
		builder->addPlace(p->name, p->tokens); // Ignore coordinates

		// Build variables for token ages
		for(int i = 0; i < bound; i++)
			builder->addVariable(tokenAgeVariable(p->name, i), 0, p->maxAge);

		// Build post-places
		if(isEndPlace(p->name)){
			string postplace = postPlace(p->name);
			builder->addPlace(postplace, 0);
			// Add a transition for each variable that can store the token age
			for(int i = 0; i < bound; i++){
				string postptrans = postPlaceTransition(p->name, i);
				string cond = p->name + " == " + i2s(i);
				string assign = "L := L-1; " + tokenAgeVariable(p->name, i) + " := 0;";
				builder->addTransition(postptrans, cond, assign);
				builder->addInputArc(postplace, postptrans);
				builder->addOutputArc(postptrans, p->name);
			}
		}
	}

	// For each transition
	for(TransitionIter t = transitions.begin(); t != transitions.end(); t++){
		// Find pre and post sets
		InArcList  pre  = preset(t->name);
		OutArcList post = postset(t->name);

		// Build original transition, remember to set release lock
		string assign = "L := " + i2s(lockStateIdle) + "; R := " + i2s(post.size()) + ";";
		builder->addTransition(t->name, "", assign);

		// For each in-arc
		int inArcNr = 0;
		for(InArcIter ai = pre.begin(); ai != pre.end(); ai++){
			// Build pre-place
			string preplace = prePlace(t->name, inArcNr);
			builder->addPlace(preplace, 0);
			// Build k new pre-place-transitions
			for(int i = 0; i < bound; i++){
				string pretrans = prePlaceTransition(t->name, inArcNr, i);

				// Create pre-condition
				string conds;
				// preplace == 0
				conds += preplace + " == 0 and \n";
				// ai->start >= i
				conds += ai->start + " >= " + i2s(i) + " and \n";
				// ai->startInterval <= <ai->start>_<i>
				conds += i2s(ai->startInterval) + " <= " + tokenAgeVariable(ai->start, i) + " and \n";
				// <ai->start>_<i> <= ai->endInterval
				conds += tokenAgeVariable(ai->start, i) + " <= " + i2s(ai->endInterval) + " and \n";
				// L == LockStateIdle or L == lockState(t->name)
				conds += "( L == " + i2s(lockStateIdle) + " or L == " + lockState(t->name) + " ) and \n";
				// Release lock clean
				conds += "R == 0";

				// Create post-assignments
				string assigns;
				// L := lockState(t->name)
				assigns += "L := " + lockState(t->name) + " ; \n";
				// Shift the values of variables with higher index
				for(int j = i; j < bound - 1; j++){
					// <place>_<j>   := <place>_<j+1>
					assigns += tokenAgeVariable(ai->start,j);
					assigns += " := " + tokenAgeVariable(ai->start,j + 1) + " ; \n";
				}
				builder->addTransition(pretrans, conds, assigns);

				// Add arc from original place to new transition
				builder->addInputArc(ai->start, pretrans);
				// Add arc from transition to pre-place.
				builder->addOutputArc(pretrans, preplace);
			}
			// Connect pre-place to original transition
			builder->addInputArc(preplace, t->name);
			inArcNr++;
		}

		// Create arcs to post-places
		for(OutArcIter ai = post.begin(); ai != post.end(); ai++)
			builder->addOutputArc(t->name, postPlace(ai->end));
	}

	// Create control net (for ageing)
	int marking = 1;
	for(PlaceIter p = places.begin(); p != places.end(); p++){
		for(int i = 0; i < bound; i++){
			// Build intermediate places
			builder->addPlace(intermediateAgeingPlace(p->name, i), marking);
			marking = 0;
		}
	}
	bool isFirst = true;
	for(PlaceIter p = places.begin(); p != places.end(); p++){
		for(int i = 0; i < bound; i++){
			// Determine if we're last
			bool isLast = false;
			int ni = i + 1;
			PlaceIter np = p;
			if(i == bound-1){
				np++;
				ni = 0;
				if(np == places.end()){
					np = places.begin();
					isLast = true;
				}
			}
			string iplace = intermediateAgeingPlace(p->name, i);
			string niplace = intermediateAgeingPlace(np->name, ni);

			string tokenAge = tokenAgeVariable(p->name, i);
			string maxCond, ageCond, maxAssign, ageAssign;
			// If we are the first place, require that the lock is idle, and set it ageing
			if(isFirst){
				isFirst = false;
				maxCond += "L == " + i2s(lockStateIdle) + " and ";
				ageCond += "L == " + i2s(lockStateIdle) + " and ";
				maxAssign += "L := " + i2s(lockStateAgeing) + " ; ";
				ageAssign += "L := " + i2s(lockStateAgeing) + " ; ";
			}else if(isLast){
				// If we're last, set lock state idle
				maxAssign += "L := " + i2s(lockStateIdle) + " ; ";
				ageAssign += "L := " + i2s(lockStateIdle) + " ; ";
			}
			// Increment token age if not max
			maxCond += tokenAge + " == " + i2s(p->maxAge);
			ageCond += tokenAge + " < " + i2s(p->maxAge);
			ageAssign += tokenAge + " := " + tokenAge + " + 1 ;";
			// Create intermediate ageing transitions
			string maxT = maxTransition(p->name, i);
			string ageT = ageTransition(p->name, i);
			// Create network structure
			builder->addTransition(maxT, maxCond, maxAssign);
			builder->addTransition(ageT, ageCond, ageAssign);
			// Connect places and transitions
			builder->addInputArc(iplace, maxT);
			builder->addInputArc(iplace, ageT);
			builder->addOutputArc(maxT, niplace);
			builder->addOutputArc(ageT, niplace);
		}
	}
}

/** Lock state for a transition */
string DTAPNTranslator::lockState(const string& transition){
	int next = lockStateIdle + 1;
	for(TransitionIter t = transitions.begin(); t != transitions.end(); t++){
		if(t->name == transition)
			return i2s(next);
		next++;
	}
	assert(false);
	return "";
}

/** Max transition between two intermediate ageing places */
string DTAPNTranslator::maxTransition(const string& place, int tokenIndex){
	return place + "_max_t_" + i2s(tokenIndex);
}

/** Age transition between two intermediate ageing places */
string DTAPNTranslator::ageTransition(const string& place, int tokenIndex){
	return place + "_age_t_" + i2s(tokenIndex);
}

/** An intermediate ageing place*/
string DTAPNTranslator::intermediateAgeingPlace(const string& place, int tokenIndex){
	return place + "_age_" + i2s(tokenIndex);
}

/** Pre place to a transition */
string DTAPNTranslator::prePlace(const string& transition, int inArcNr){
	return transition + "_pre_" + i2s(inArcNr);
}

/** Transition from place to pre-place */
string DTAPNTranslator::prePlaceTransition(const string& transition, int inArcNr, int tokenIndex){
	return transition + "_pre_t_" + i2s(inArcNr) + "_" + i2s(tokenIndex);
}

/** Post-place from a transition, only one per place */
string DTAPNTranslator::postPlace(const string& place){
	return place + "_post";
}

/** Transition from post-place to place */
string DTAPNTranslator::postPlaceTransition(const string& place, int tokenIndex){
	return place + "_t" + i2s(tokenIndex);
}

/** Variable representing the age of a specific token at a place */
string DTAPNTranslator::tokenAgeVariable(const string& place, int tokenIndex){
	return place + "_" + i2s(tokenIndex);
}

} // DTAPN
} // PetriEngine

