
#include <UnitTest++.h>
#include "PetriNet.h"
#include "PQL/PQLParser.h"
#include "PQL/PQL.h"
#include "PQL/CompiledCondition.h"

using namespace PetriEngine;
using namespace PetriEngine::PQL;
using namespace std;

SUITE(CompiledConditionTest){
	TEST(SimpleCompiledConditionTest){
		Condition* cond = ParseQuery("P1 == 10 or P2 == 5");
		vector<string> places;
		vector<string> vars;
		places.push_back("P1");
		places.push_back("P2");
		AnalysisContext a(places, vars);

		CompiledCondition* C = new CompiledCondition(cond);

		C->analyze(a);
		CHECK(a.errors().size() == 0);
		CHECK(C->compile());

		MarkVal m1[2];
		m1[0] = 9;
		m1[1] = 5;
		CHECK(C->evaluate(EvaluationContext(m1, NULL)));
		m1[0] = 9;
		m1[1] = 4;
		CHECK(!C->evaluate(EvaluationContext(m1, NULL)));
		m1[0] = 10;
		m1[1] = 3;
		CHECK(C->evaluate(EvaluationContext(m1, NULL)));
	}
}
