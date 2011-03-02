#include <UnitTest++.h>
#include <string>
#include <iostream>
#include "CTL/CTLParser.h"
#include "PetriNetFactory.h"

using namespace PetriEngine;
using namespace PetriEngine::CTL;
using namespace std;

SUITE(CTLParserTest){

	TEST(CTLTreeConstruction){

		string query("P1==1&&P2>1*(P2+5)");

		PetriNetFactory fac;
		fac.addPlace("P1",0,0);
		fac.addPlace("P2",1,1);
		fac.addTransition("T0",0.5,0.5);
		fac.addInputArc("P1","T0",1);
		fac.addOutputArc("T0","P2",1);

		PetriNet net = *(fac.makePetriNet());

		CTLParser parser(&net);
		CTLExpr* expr1 = parser.parse(query);
		CTLExpr* expr2 = parser.parse(expr1->toString());
		CHECK(expr1->toString() == expr2->toString());
	}

	TEST(CTLEvaluation){

		string queryString("P1==0&&P2==1");

		PetriNetFactory fac;
		fac.addPlace("P1",0,0);
		fac.addPlace("P2",1,1);
		fac.addTransition("T0",0.5,0.5);
		fac.addInputArc("P1","T0",1);
		fac.addOutputArc("T0","P2",1);

		PetriNet net = *(fac.makePetriNet());

		CTLParser parser(&net);
		CTLExpr* query = parser.parse(queryString);

		Mark* m0 = net.makeEmptyMarking();
		CHECK(query->evaluate(m0) == 0);

		Mark* m1 = net.makeEmptyMarking();
		m0[0] = 1;
		net.fire(0,m0,m1);
		CHECK(query->evaluate(m1) != 0);
	}
}
