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
#include "LayoutBuilder.h"

#include <igraph/igraph.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

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

bool attrTableAttached = false;

void LayoutBuilder::produce(AbstractPetriNetBuilder *builder){
	if(!attrTableAttached){
		igraph_i_set_attribute_table(&igraph_cattribute_table);
		attrTableAttached = true;
	}
	size_t V = places.size() + transitions.size();
	size_t E = inArcs.size() + outArcs.size();
	igraph_t graph;
	// Create a directed graph
	igraph_empty(&graph, V, true);

	// Create vector with all edges
	igraph_vector_t edges;
	igraph_vector_init(&edges, E * 2);

	// Add edges to vector
	int i = 0;
	for(ArcIter it = inArcs.begin(); it != inArcs.end(); it++){
		VECTOR(edges)[i++] = numberFromName(it->start);
		VECTOR(edges)[i++] = numberFromName(it->end);
	}
	for(ArcIter it = outArcs.begin(); it != outArcs.end(); it++){
		VECTOR(edges)[i++] = numberFromName(it->start);
		VECTOR(edges)[i++] = numberFromName(it->end);
	}

	// Add the edges to graph
	igraph_add_edges(&graph, &edges, 0);

	// Delete the vector with edges
	igraph_vector_destroy(&edges);

	// Arrays to store result in
	double posx[V];
	double posy[V];

	// Provide current positions, if they're used at all
	if(startFromCurrentPositions){
		int i = 0;
		for(PlaceIter it = places.begin(); it != places.end(); it++){
			posx[i] = it->x;
			posy[i] = it->y;
			igraph_cattribute_VAN_set(&graph, "id", i, i);
			i++;
		}
		for(TransitionIter it = transitions.begin(); it != transitions.end(); it++){
			posx[i] = it->x;
			posy[i] = it->y;
			igraph_cattribute_VAN_set(&graph, "id", i, i);
			i++;
		}
	}

	// Decompose the graph, and layout subgraphs induvidually
	igraph_vector_ptr_t subgraphs;
	igraph_vector_ptr_init(&subgraphs, 0);
	igraph_decompose(&graph, &subgraphs, IGRAPH_WEAK, -1, 0);

	// Offset for places subgraphs
	double offsetx = 0;
	double offsety = 0;

	// Layout, translate and extract results for each subgraph
	for(int i = 0; i < igraph_vector_ptr_size(&subgraphs); i++){
		//Get the subgraph
		igraph_t* subgraph = (igraph_t*)VECTOR(subgraphs)[i];

		// Allocate result matrix
		igraph_matrix_t sublayout;
		igraph_matrix_init(&sublayout, 0, 0);

		// Vertex selector and iterator
		igraph_vs_t vs;
		igraph_vit_t vit;
		// Select all and create iterator
		igraph_vs_all(&vs);
		igraph_vit_create(subgraph, vs, &vit);

		// Initialize sublayout, using original positions
		if(startFromCurrentPositions){
			// Count vertices
			int vertices = 0;
			// Iterator over vertices to count them, hacked but it works
			while(!IGRAPH_VIT_END(vit)){
				vertices++;
				IGRAPH_VIT_NEXT(vit);
			}
			//Reset vertex iterator
			IGRAPH_VIT_RESET(vit);
			// Resize sublayout
			igraph_matrix_resize(&sublayout, vertices, 2);
			// Iterator over vertices
			while(!IGRAPH_VIT_END(vit)){
				int subindex = (int)IGRAPH_VIT_GET(vit);
				int index = (int)igraph_cattribute_VAN(subgraph, "id", subindex);
				MATRIX(sublayout, subindex, 0) = posx[index];
				MATRIX(sublayout, subindex, 1) = posy[index];
				IGRAPH_VIT_NEXT(vit);
			}
			//Reset vertex iterator
			IGRAPH_VIT_RESET(vit);
		}

		igraph_layout_kamada_kawai(subgraph, &sublayout, 1000, ((double)V)/4.0, 10, 0.99, V*V, startFromCurrentPositions);
		// Other layout algorithms with reasonable parameters
		//igraph_layout_kamada_kawai(subgraph, &sublayout, 1000, ((double)V)/4.0, 10, 0.99, V*V, startFromCurrentPositions);
		//igraph_layout_grid_fruchterman_reingold(subgraph, &sublayout, 500, V, V*V, 1.5, V*V*V, V*V/4, startFromCurrentPositions);
		//igraph_layout_fruchterman_reingold(subgraph, &sublayout, 500, V, V*V, 1.5, V*V*V, startFromCurrentPositions, NULL);
		//igraph_layout_lgl(subgraph, &sublayout, 150, V, V*V, 1.5, V*V*V, sqrt(V), -1);

		//Find min and max values:
		double minx = DBL_MAX,
			   miny = DBL_MAX,
			   maxx = -DBL_MAX,
			   maxy = -DBL_MAX;
		//Iterator over all vertices
		while(!IGRAPH_VIT_END(vit)){
			int subindex = (int)IGRAPH_VIT_GET(vit);
			double x = MATRIX(sublayout, subindex, 0) * factor;
			double y = MATRIX(sublayout, subindex, 1) * factor;
			minx = minx < x ? minx : x;
			miny = miny < y ? miny : y;
			maxx = maxx > x ? maxx : x;
			maxy = maxy > y ? maxy : y;
			IGRAPH_VIT_NEXT(vit);
		}
		//Reset vertex iterator
		IGRAPH_VIT_RESET(vit);

		// Compute translation
		double tx = margin - minx;
		double ty = margin - miny;
		// Decide whether to put it below or left of current content
		if(maxx - minx + offsetx < maxy - miny + offsety){
			tx += offsetx;
			offsetx += maxx - minx + margin;
			if(offsety < maxy - miny + margin)
				offsety = maxy - miny + margin;
		}else{
			ty += offsety;
			offsety += maxy - miny + margin;
			if(offsetx < maxx - minx + margin)
				offsetx = maxx - minx + margin;
		}
		// Translate and extract results
		while(!IGRAPH_VIT_END(vit)){
			int subindex = (int)IGRAPH_VIT_GET(vit);
			int index = (int)igraph_cattribute_VAN(subgraph, "id", subindex);
			double x = MATRIX(sublayout, subindex, 0) * factor;
			double y = MATRIX(sublayout, subindex, 1) * factor;
			posx[index] = x + tx;
			posy[index] = y + ty;
			IGRAPH_VIT_NEXT(vit);
		}
		// Destroy iterator and selector
		igraph_vit_destroy(&vit);
		igraph_vs_destroy(&vs);

		// Destroy the sublayout
		igraph_matrix_destroy(&sublayout);

		// Destroy subgraph
		igraph_destroy(subgraph);
		free(VECTOR(subgraphs)[i]);
	}

	// Remove the attributes
	igraph_cattribute_remove_v(&graph, "id");

	// Destroy the graph
	igraph_destroy(&graph);

	// Insert results
	i = 0;
	for(PlaceIter it = places.begin(); it != places.end(); it++){
		it->x = posx[i];
		it->y = posy[i];
		i++;
	}
	for(TransitionIter it = transitions.begin(); it != transitions.end(); it++){
		it->x = posx[i];
		it->y = posy[i];
		i++;
	}

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
