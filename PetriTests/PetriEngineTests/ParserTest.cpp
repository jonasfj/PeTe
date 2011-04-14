#include <UnitTest++.h>
#include <string>
#include <iostream>
#include "PQL/PQLParser.h"
#include "PQL/PQL.h"
#include "PetriNetBuilder.h"

using namespace PetriEngine;
using namespace PetriEngine::PQL;
using namespace std;

SUITE(PQLParserTest){
	TEST(PQLQueryTreeConstruction){
		string query("(P5==5 && P1 > 4)");

		Condition* expr1 = ParseQuery(query);
		CHECK(expr1 != NULL);
		//cout<<"Expression parsed:"<<endl;
		//cout<<expr1->toString()<<endl;

		Condition* expr2 = ParseQuery(expr1->toString());
		CHECK(expr1->toString() == expr2->toString());
	}

	TEST(PQLAssignmentTreeConstruction){
		//This works
		string assignment("X2:=5;Excaliber:=4+5*8+(55*7)+1;");

		AssignmentExpression* ass1 = ParseAssignment(assignment);
		CHECK(ass1 != NULL);

		AssignmentExpression* ass2 = ParseAssignment(ass1->toString());
		CHECK(ass1->toString() == ass2->toString());

		//This shouldn't
		//string assignment1("X2:=5:Excaliber:=4+5*8+(55*7)+1;");

		//AssignmentExpression* ass3 = ParseAssignment(assignment1);
		//CHECK(ass1 == NULL);

	}
/*
	TEST(CTLEvaluation){

		string queryString("PdfsHash1==0&&P2==1");

		PetriNetBuilder builder;
		builder.addPlace("P1",0,0,0);
		builder.addPlace("P2",0,1,1);
		builder.addTransition("T0",0.5,0.5);
		builder.addInputArc("P1","T0",1);
		builder.addOutputArc("T0","P2",1);

		PetriNet net = *(builder.makePetriNet());

		CTLParser parser(&net);
		CTLExpr* query = parser.parse(queryString);

		Mark* m0 = net.makeEmptyMarking();
		CHECK(query->evaluate(m0) == 0);

		Mark* m1 = net.makeEmptyMarking();
		m0[0] = 1;
		net.fire(0,m0,m1);
		CHECK(query->evaluate(m1) != 0);
	}
*/
}
