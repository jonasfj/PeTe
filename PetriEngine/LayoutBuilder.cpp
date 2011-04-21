#include "LayoutBuilder.h"

#include <igraph/igraph.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

namespace PetriEngine{

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

void free_subgraphs(igraph_vector_ptr_t *subgraphs) {
  long int i;
  for (i=0; i<igraph_vector_ptr_size(subgraphs); i++) {
	igraph_destroy((igraph_t*)VECTOR(*subgraphs)[i]);
	free(VECTOR(*subgraphs)[i]);
  }
}

bool attrTableAttached = false;

void LayoutBuilder::produce(AbstractPetriNetBuilder *builder){
	if(!attrTableAttached){
		igraph_i_set_attribute_table(&igraph_cattribute_table);
		attrTableAttached = true;
	}
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
			MATRIX(pos, i, 0) = (int)it->x;
			MATRIX(pos, i, 1) = (int)it->y;
			igraph_cattribute_VAN_set(&graph, "id", i, i);
			i++;
		}
		for(TransitionIter it = transitions.begin(); it != transitions.end(); it++){
			MATRIX(pos, i, 0) = (int)it->x;
			MATRIX(pos, i, 1) = (int)it->y;
			igraph_cattribute_VAN_set(&graph, "id", i, i);
			i++;
		}
	}

	// test decomposition


	igraph_vector_ptr_t subgraphs;
	igraph_vector_ptr_init(&subgraphs, 0);
	igraph_decompose(&graph, &subgraphs, IGRAPH_WEAK, -1, 0);

	fprintf(stderr, "Decompositions: %i\n", igraph_vector_ptr_size(&subgraphs));

	for(int i = 0; i < igraph_vector_ptr_size(&subgraphs); i++){
		igraph_t* g = (igraph_t*)VECTOR(subgraphs)[i];
		fprintf(stderr, "Subgraph %i: %i\n", i, igraph_vcount((igraph_t*)VECTOR(subgraphs)[i]));
		fprintf(stderr, "Subgraph %i: %i\n", i, igraph_ecount((igraph_t*)VECTOR(subgraphs)[i]));
		igraph_write_graph_edgelist((igraph_t*)VECTOR(subgraphs)[i], stderr);

		// Vertex selector
		igraph_vs_t vs;
		// Vertex iterator
		igraph_vit_t vit;
		igraph_vs_all(&vs);
		igraph_vit_create(g, vs, &vit);

		while(!IGRAPH_VIT_END(vit)){
			fprintf(stderr, "%li = %i \n", (long int)IGRAPH_VIT_GET(vit),
										(int)igraph_cattribute_VAN(g, "id", IGRAPH_VIT_GET(vit)));
			IGRAPH_VIT_NEXT(vit);
		}
		igraph_vit_destroy(&vit);
		igraph_vs_destroy(&vs);

		/*// Allocate result matrix
		igraph_matrix_t pos2;
		// I assume this is the right size
		igraph_matrix_init(&pos2, 0, 0);
		igraph_layout_kamada_kawai(&graph, &pos2, 1000, ((double)N)/4.0, 10, 0.99, N*N, false);
		*/

		fprintf(stderr, "-----\n");
	}



	free_subgraphs(&subgraphs);


	// test decomposition

	// Run kamada kawai, with reasonable parameters
	igraph_layout_kamada_kawai(&graph, &pos, 1000, ((double)N)/4.0, 10, 0.99, N*N, startFromCurrentPositions);
	//igraph_layout_grid_fruchterman_reingold(&graph, &pos, 500, N, N*N, 1.5, N*N*N, N*N/4, startFromCurrentPositions);
	//igraph_layout_fruchterman_reingold(&graph, &pos, 500, N, N*N, 1.5, N*N*N, startFromCurrentPositions, NULL);
	//igraph_layout_lgl(&graph, &pos, 150, N, N*N, 1.5, N*N*N, sqrt(N), -1);

	// Extract results
	i = 0;
	double minx = 0, miny = 0;
	for(PlaceIter it = places.begin(); it != places.end(); it++){
		it->x = (double)MATRIX(pos, i, 0) * factor;
		it->y = (double)MATRIX(pos, i, 1) * factor;
		minx = minx < it->x ? minx : it->x;
		miny = miny < it->y ? miny : it->y;
		i++;
	}
	for(TransitionIter it = transitions.begin(); it != transitions.end(); it++){
		it->x = (double)MATRIX(pos, i, 0) * factor;
		it->y = (double)MATRIX(pos, i, 1) * factor;
		minx = minx < it->x ? minx : it->x;
		miny = miny < it->y ? miny : it->y;
		i++;
	}
	//Translated the coordinates
	double tx = margin - minx;
	double ty = margin - minx;
	for(PlaceIter it = places.begin(); it != places.end(); it++){
		it->x += tx;
		it->y += ty;
	}
	for(TransitionIter it = transitions.begin(); it != transitions.end(); it++){
		it->x += tx;
		it->y += ty;
	}

	// Destroy the result
	igraph_matrix_destroy(&pos);

	// Destroy the attributes
	igraph_cattribute_remove_v(&graph, "id");

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
