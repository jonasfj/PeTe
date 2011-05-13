/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
	assert(!name.empty());
	Place p;
	p.name = name;
	p.tokens = tokens;
	p.maxAge = 0;
	places.push_back(p);
}

void DTAPNTranslator::addTransition(const std::string& name, double, double){
	assert(!name.empty());
	Transition t;
	t.name = name;
	transitions.push_back(t);
}

bool DTAPNTranslator::hasIdentifier(const std::string& id){
	for(PlaceIter p = places.begin(); p != places.end(); p++)
		if(p->name == id)
			return true;
	for(TransitionIter t = transitions.begin(); t != transitions.end(); t++)
		if(t->name == id)
			return true;
	return false;
}

void DTAPNTranslator::addInputArc(const std::string& place, const std::string& transition, int startInterval, int endInterval){
	InArc a;
	a.start = place;
	a.end = transition;
	a.startInterval = startInterval;
	a.endInterval = endInterval;
	inArcs.push_back(a);
}

void DTAPNTranslator::addOutputArc(const std::string& transition, const std::string& place){
	OutArc a;
	a.start = transition;
	a.end = place;
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
std::string DTAPNTranslator::escapeIdentifier(std::string identifier){
	while(hasIdentifier(identifier))
		identifier += "_";
	return identifier;
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
		p.maxAge = MAX(p.maxAge, MAX(ai->startInterval, ai->endInterval + 1));
	}

	// Replace infinity with max age
	for(InArcIter ai = inArcs.begin(); ai != inArcs.end(); ai++){
		Place& p = findPlace(ai->start);
		if(ai->endInterval == -1)
			ai->endInterval = p.maxAge;
	}

	// Build control variables (Transition Lock and Release Lock)
	lockStateIdle = 0;
	lockStateAgeing = transitions.size()+1;
	builder->addVariable("L", 0, transitions.size() + 1); //one for each transition + idle and ageing

	// For each place
	for(PlaceIter p = places.begin(); p != places.end(); p++){
		// Build original place
		builder->addPlace(p->name, p->tokens); // Ignore coordinates

		// Build variables for token ages
		for(int i = 0; i < bound; i++)
			builder->addVariable(tokenAgeVariable(p->name, i), 0, p->maxAge);
	}

	// For each transition
	for(TransitionIter t = transitions.begin(); t != transitions.end(); t++){
		// Find pre and post sets
		InArcList  pre  = preset(t->name);
		OutArcList post = postset(t->name);

		// Build original transition, remember to set release lock
		string assign = "L := " + i2s(lockStateIdle) + "; \n";
		// Create result assign
		for(OutArcIter ai = post.begin(); ai != post.end(); ai++){
			assign += tokenAgeVariable(ai->end, 0) + " := 0 ; \n";
			for(int i = 1; i < bound-1; i++)
				assign += tokenAgeVariable(ai->end, i) + " := "  + tokenAgeVariable(ai->end, i-1) + " ; \n";
		}

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
				conds += "( L == " + i2s(lockStateIdle) + " or L == " + lockState(t->name) + " )";

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
			builder->addOutputArc(t->name, ai->end);
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
				//maxCond += "L == " + i2s(lockStateIdle) + " and "; Not necessary with these
				//ageCond += "L == " + i2s(lockStateIdle) + " and ";
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
	return escapeIdentifier(place + "_max_t_" + i2s(tokenIndex));
}

/** Age transition between two intermediate ageing places */
string DTAPNTranslator::ageTransition(const string& place, int tokenIndex){
	return escapeIdentifier(place + "_age_t_" + i2s(tokenIndex));
}

/** An intermediate ageing place*/
string DTAPNTranslator::intermediateAgeingPlace(const string& place, int tokenIndex){
	return escapeIdentifier(place + "_age_" + i2s(tokenIndex));
}

/** Pre place to a transition */
string DTAPNTranslator::prePlace(const string& transition, int inArcNr){
	return escapeIdentifier(transition + "_pre_" + i2s(inArcNr));
}

/** Transition from place to pre-place */
string DTAPNTranslator::prePlaceTransition(const string& transition, int inArcNr, int tokenIndex){
	return escapeIdentifier(transition + "_pre_t_" + i2s(inArcNr) + "_" + i2s(tokenIndex));
}

/** Variable representing the age of a specific token at a place */
string DTAPNTranslator::tokenAgeVariable(const string& place, int tokenIndex){
	return escapeIdentifier(place + "_" + i2s(tokenIndex));
}

/** Translates a DTAPN query into a PNDV query */
string DTAPNTranslator::translateQuery(const std::string &query){
	return "L == 0 and ( "+ query + " ) ";
}

} // DTAPN
} // PetriEngine

