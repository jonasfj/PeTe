#include <UnitTest++.h>
#include "DepthFirstReachabilitySearch.h"
#include "StandardOutputProgressReporter.h"
#include "PetriNetFactory.h"

using namespace PetriEngine;

SUITE(ReachabilityStrategyTest){

	/*TEST(DFSReachabilityTest){

		DepthFirstReachabilitySearch dfs;

		PetriNetFactory fac;
		fac.addPlace("P1",0,0);
		fac.addPlace("P2",1,1);
		fac.addTransition("T0",0.5,0.5);
		fac.addInputArc("P1","T0",1);
		fac.addOutputArc("T0","P2",1);

		// Unbounded test
		fac.addTransition("T1",0.5,0.5);
		fac.addInputArc("P2","T1",1);
		fac.addOutputArc("T1","P1",1);

		PetriNet net = *(fac.makePetriNet());

		Mark* m = new Mark[net.nPlaces()];
		m[0] = 1;

		dfs.reachable(net,m);
	}*/

	TEST(DFSReachabilityTest2){
		// Tests another unbounded net
		DepthFirstReachabilitySearch dfs;

		PetriNetFactory fac;

		fac.addPlace("P1",0,0);
		fac.addPlace("P2",1,1);
		fac.addPlace("P3",2,2);

		fac.addTransition("T0", 0.5, 0.5);
		fac.addTransition("T1", 0,0);
		fac.addTransition("T2", 0,0);

		fac.addInputArc("P1", "T0", 1);
		fac.addOutputArc("T0", "P2", 1);
		fac.addOutputArc("T0", "P3", 1);
		fac.addInputArc("P2", "T1", 1);
		fac.addInputArc("P3", "T2", 1);
		fac.addOutputArc("T1", "P1", 1);
		fac.addOutputArc("T2", "P1", 1);

		PetriNet net = *(fac.makePetriNet());

		Mark* m = net.makeEmptyMarking();
		m[0] = 1;

		dfs.reachable(net,m);
	}

	TEST(PetriNetFireTest){

		PetriNetFactory fac;
		fac.addPlace("P1",0,0);
		fac.addPlace("P2",1,1);
		fac.addTransition("T0",0.5,0.5);
		fac.addInputArc("P1","T0",1);
		fac.addOutputArc("T0","P2",1);

		PetriNet net = *(fac.makePetriNet());
		Mark* m = net.makeEmptyMarking();
		Mark* m2 = net.makeEmptyMarking();
		m[0] = 1;
		// Initial M(P1) = 1, M(P2) = 0
		CHECK(m[0] == 1);
		CHECK(m[1] == 0);

		// Fire T0
		CHECK(net.fire(0,m,m2));

		// New M(P1) = 0, M(P2) = 1
		CHECK(m2[0] == 0);
		CHECK(m2[1] == 1);
	}

}
