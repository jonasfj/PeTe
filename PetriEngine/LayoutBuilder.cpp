#include "LayoutBuilder.h"

#include <igraph/igraph.h>
#include <assert.h>

namespace PetriEngine{

LayoutBuilder::LayoutBuilder(){
	startFromCurrentPositions = true;
}

void LayoutBuilder::addVariable(const std::string &name, int initialValue, int range){
	Var v;
	v.name = name;
	v.initialValue = initialValue;
	v.range = range;
	vars.push_back(v);
}

void LayoutBuilder::addPlace(const std::string &name, int tokens, double, double){
	Place p;
	p.name = name;
	p.tokens = tokens;
	places.push_back(p);
}

void LayoutBuilder::addTransition(const std::string &name, const std::string &conditions, const std::string &assignments, double, double){
	Transition t;
	t.name = name;
	t.conditions = conditions;
	t.assignments = assignments;
	transitions.push_back(t);
}

void LayoutBuilder::addInputArc(const std::string &place, const std::string &transition, int weight){
	Arc a;
	a.start = place;
	a.end = transition;
	a.weight = weight;
	inArcs.push_back(a);
}

void LayoutBuilder::addOutputArc(const std::string &transition, const std::string &place, int weight){
	Arc a;
	a.start = transition;
	a.end = place;
	a.weight = weight;
	outArcs.push_back(a);
}

int LayoutBuilder::numberFromName(const std::string& name){
	int i = 0;
	for(PlaceIter it = places.begin(); it != places.end(); it++){
		if(it->name == name)
			return i;
		i++;
	}
	for(TransitionIter it = transitions.begin(); it != transitions.end(); it++){
		if(it->name == name)
			return i;
		i++;
	}
	assert(false); //Please validate before running this!
	return -1;
}

void LayoutBuilder::produce(AbstractPetriNetBuilder *builder){
	size_t N = places.size() + transitions.size();
	size_t V = inArcs.size() + outArcs.size();
	igraph_t graph;
	// Create a directed graph
	igraph_empty(&graph, N, true);

	// Create vector with all edges
	igraph_vector_t edges;
	igraph_vector_init(&edges, V * 2);

	int i = 0;
	for(ArcIter it = inArcs.begin(); it != inArcs.end(); it++){
		VECTOR(edges)[i++] = numberFromName(it->start);
		VECTOR(edges)[i++] = numberFromName(it->end);
	}
	for(ArcIter it = outArcs.begin(); it != outArcs.end(); it++){
		VECTOR(edges)[i++] = numberFromName(it->start);
		VECTOR(edges)[i++] = numberFromName(it->end);
	}

	// Add the edges
	igraph_add_edges(&graph, &edges, 0);

	// Delete the vector with edges
	igraph_vector_destroy(&edges);

	// Allocate result matrix
	igraph_matrix_t pos;

	// I assume this is the right size
	igraph_matrix_init(&pos, N, 2);

	// Provide current positions, if someone wants to use them
	if(startFromCurrentPositions){
		int i = 0;
		for(PlaceIter it = places.begin(); it != places.end(); it++){
			MATRIX(pos, i, 0) = it->x;
			MATRIX(pos, i, 1) = it->y;
			i++;
		}
		for(TransitionIter it = transitions.begin(); it != transitions.end(); it++){
			MATRIX(pos, i, 0) = it->x;
			MATRIX(pos, i, 1) = it->y;
			i++;
		}
	}

	// Run kamada kawai, with reasonable parameters
	igraph_layout_kamada_kawai(&graph, &pos, 1000, N/4, 10, 0.99, V*V, startFromCurrentPositions);

	// Extract results
	i = 0;
	for(PlaceIter it = places.begin(); it != places.end(); it++){
		it->x = MATRIX(pos, i, 0);
		it->y = MATRIX(pos, i, 1);
		i++;
	}
	for(TransitionIter it = transitions.begin(); it != transitions.end(); it++){
		it->x = MATRIX(pos, i, 0);
		it->y = MATRIX(pos, i, 1);
		i++;
	}

	// Destroy the result
	igraph_matrix_destroy(&pos);

	// Destroy the graph
	igraph_destroy(&graph);


	// Produce variables
	for(VarIter it = vars.begin(); it != vars.end(); it++)
		builder->addVariable(it->name, it->initialValue, it->range);

	for(PlaceIter it = places.begin(); it != places.end(); it++)
		builder->addPlace(it->name, it->tokens, it->x, it->y);

	for(TransitionIter it = transitions.begin(); it != transitions.end(); it++)
		builder->addTransition(it->name, it->conditions, it->assignments, it->x, it->y);

	for(ArcIter it = inArcs.begin(); it != inArcs.end(); it++)
		builder->addInputArc(it->start, it->end, it->weight);

	for(ArcIter it = outArcs.begin(); it != outArcs.end(); it++)
		builder->addInputArc(it->start, it->end, it->weight);

	//Reset builder state (just in case some idoit decides to reuse it!
	vars.clear();
	places.clear();
	transitions.clear();
	inArcs.clear();
	outArcs.clear();
}

} // PetriEngine
