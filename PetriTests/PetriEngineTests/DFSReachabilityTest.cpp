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
#include <UnitTest++.h>
#include <string>
#include <iostream>
#include <sstream>
#include "PQL/PQLParser.h"
#include "PQL/PQL.h"
#include "PQL/Contexts.h"
#include "PetriNet.h"
#include "PetriNetBuilder.h"
#include "Reachability/DepthFirstReachabilitySearch.h"
#include "Structures/StateSet.h"
#include "Structures/StateAllocator.h"


using namespace PetriEngine;
using namespace PetriEngine::Structures;
using namespace PetriEngine::Reachability;


SUITE(DepthFirstReachabilitySearch) {

TEST(Test1){
	//Construct PetriNet
	PetriNetBuilder builder;
	//Add places
	builder.addPlace("P0", 3, 1, 1);
	builder.addPlace("P1", 0, 1, 1);
	builder.addPlace("P2", 0, 1, 1);
	builder.addPlace("P3", 0, 1, 1);
	//Add transitions
	builder.addTransition("T1", "", "", 1, 1);
	builder.addTransition("T2", "", "", 1, 1);
	//Add arcs
	builder.addInputArc("P0", "T1", 1);
	builder.addInputArc("P1", "T2", 1);
	builder.addInputArc("P2", "T2", 1);
	builder.addOutputArc("T1", "P1", 1);
	builder.addOutputArc("T1", "P2", 1);
	builder.addOutputArc("T1", "P3", 1);

	PetriNet* net = builder.makePetriNet();
	MarkVal* marking = builder.makeInitialMarking();
	VarVal* valuation = builder.makeInitialAssignment();

	//Satisfiable queries
	std::string query1("P0 == 3 and P3 == 0");
	std::string query2("P1 == 1 and P2 == 1");
	std::string query3("P1 == 1 and P3 == 2");
	//Not satisfiable queries
	std::string nQuery1("P3 > 4 or (P1 == 3 and P3 < 0)");
	std::string nQuery2("P1 == 2 and P2 == 1");

	PQL::AnalysisContext aContext(*(net));
	PQL::EvaluationContext eContext(marking, valuation);
	DepthFirstReachabilitySearch dfs;
	ReachabilityResult result;

	//Query1
	PQL::Condition* expr = PQL::ParseQuery(query1);
	expr->analyze(aContext);
	result = dfs.reachable(*(net), marking, valuation, expr);
	CHECK(result.result() == result.Satisfied);
	//Query2
	expr = PQL::ParseQuery(query2);
	expr->analyze(aContext);
	result = dfs.reachable(*(net), marking, valuation, expr);
	CHECK(result.result() == result.Satisfied);
	//Query3
	expr = PQL::ParseQuery(query3);
	expr->analyze(aContext);
	result = dfs.reachable(*(net), marking, valuation, expr);
	CHECK(result.result() == result.Satisfied);
	//NQuery1
	expr = PQL::ParseQuery(nQuery1);
	expr->analyze(aContext);
	result = dfs.reachable(*(net), marking, valuation, expr);
	CHECK(result.result() == result.NotSatisfied);
	//NQuery2
	expr = PQL::ParseQuery(nQuery2);
	expr->analyze(aContext);
	result = dfs.reachable(*(net), marking, valuation, expr);
	CHECK(result.result() == result.NotSatisfied);
}

TEST(UNORDERED_SET){
	StateAllocator<10> allocator(12, 0);
	StateSet states(12,0);
	for(int i = 0; i < 100000; i++){
		State* s0 = allocator.createState();
		s0->marking()[0] = i;
		CHECK(states.add(s0));
	}
	for(int i = 0; i < 5000; i++){
		State* s0 = allocator.createState();
		s0->marking()[0] = i;
		CHECK(!states.add(s0));
	}
}


}
