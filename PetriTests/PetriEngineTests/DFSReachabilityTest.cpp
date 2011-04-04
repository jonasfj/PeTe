#include <UnitTest++.h>
#include <string>
#include <iostream>
#include <sstream>
#include "PQL/PQLParser.h"
#include "PQL/PQLExpressions.h"
#include "PetriNet.h"
#include "PetriNetBuilder.h"
#include "Reachability/DepthFirstReachabilitySearch.h"

using namespace PetriEngine;
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

}
