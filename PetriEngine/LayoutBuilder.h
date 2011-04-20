#ifndef LAYOUTBUILDER_H
#define LAYOUTBUILDER_H

#include "AbstractPetriNetBuilder.h"

#include <string>
#include <list>

namespace PetriEngine{

/** Proxy builder for doing layout with igraph
 * Note this builder pass all other values than x and y strait through,
 * so you don't have to rebuilt your net, you can just extract the new positions.
 *
 * @remarks This builder assume the net is correct please validate first!
 */
class LayoutBuilder : public AbstractPetriNetBuilder
{
	struct Var{
		std::string name;
		int initialValue;
		int range;
	};
	typedef std::list<Var> VarList;
	typedef std::list<Var>::iterator VarIter;

	struct Place{
		std::string name;
		int tokens;
		double x, y;
	};
	typedef std::list<Place> PlaceList;
	typedef std::list<Place>::iterator PlaceIter;

	struct Transition{
		std::string name;
		std::string conditions;
		std::string assignments;
		double x, y;
	};
	typedef std::list<Transition> TransitionList;
	typedef std::list<Transition>::iterator TransitionIter;

	struct Arc{
		std::string start;
		std::string end;
		int weight;
	};
	typedef std::list<Arc> ArcList;
	typedef std::list<Arc>::iterator ArcIter;
public:
	LayoutBuilder(){
		startFromCurrentPositions = true;
		factor = 50;
		margin = 50;
	}
	void addVariable(const std::string &name, int initialValue, int range);
	void addPlace(const std::string &name, int tokens, double x, double y);
	void addTransition(const std::string &name, const std::string &conditions, const std::string &assignments, double x, double y);
	void addInputArc(const std::string &place, const std::string &transition, int weight);
	void addOutputArc(const std::string &transition, const std::string &place, int weight);
	/** Layout the net and produce it using builder */
	void produce(AbstractPetriNetBuilder* builder);
private:
	bool startFromCurrentPositions;
	int factor;
	int margin;
	VarList vars;
	PlaceList places;
	TransitionList transitions;
	ArcList inArcs, outArcs;
	/** Get place or transition number from name */
	int numberFromName(const std::string& name);
};

} // PetriEngine

#endif // LAYOUTBUILDER_H
