#ifndef DTAPNTRANSLATOR_H
#define DTAPNTRANSLATOR_H

#include <string>
#include <list>
#include <vector>

#include "AbstractPetriNetBuilder.h"

namespace PetriEngine{

/** Translator from DTAPN to PNDV */
class DTAPNTranslator
{
	/** Encapsulates a place */
	struct Place{
		std::string name;
		int tokens;
		int maxAge;
	};
	typedef std::list<Place> PlaceList;
	typedef std::list<Place>::iterator PlaceIter;
	/** Encapsulates a transition */
	struct Transition{
		std::string name;
	};
	typedef std::list<Transition> TransitionList;
	typedef std::list<Transition>::iterator TransitionIter;
	/** An input arc */
	struct InArc{
		std::string start;
		std::string end;
		int startInterval;
		int endInterval;
	};
	typedef std::list<InArc> InArcList;
	typedef std::list<InArc>::iterator InArcIter;
	/** An output arc */
	struct OutArc{
		std::string start;
		std::string end;
	};
	typedef std::list<OutArc> OutArcList;
	typedef std::list<OutArc>::iterator OutArcIter;
public:
	DTAPNTranslator(int bound){ this->bound = bound; }
	void addPlace(const std::string& name, int tokens);
	void addTransition(const std::string& name);
	void addInArc(const std::string& place, const std::string& transition, int startInterval, int endInterval);
	void addOutArc(const std::string& transition, const std::string& place);
	void makePNDV(AbstractPetriNetBuilder* builder);
private:
	/** Maximum number of tokens at a place */
	int bound;
	PlaceList places;
	TransitionList transitions;
	InArcList inArcs;
	OutArcList outArcs;
	AbstractPetriNetBuilder* builder;
	Place& findPlace(const std::string& name);
	void buildTransition(Transition& t);
	std::string escapeIdentifier(const std::string& identifier);
	InArcList preset(const std::string& transition);
	OutArcList postset(const std::string& transition);
	/** True, if place is the target of any output arcs */
	bool isEndPlace(const std::string& place);
	/** Lock state, when in idle */
	int lockStateIdle;
	/** Lock state, when in ageing */
	int lockStateAgeing;
	int lockState(const std::string& transition);
};

} // PetriEngine

#endif // DTAPNTRANSLATOR_H
