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
#include "DepthFirstReachabilitySearch.h"
#include "StandardOutputProgressReporter.h"
#include "PetriNetFactory.h"

using namespace PetriEngine;

SUITE(ReachabilityStrategyTest){

	TEST(DFSReachabilityTest){

		DepthFirstReachabilitySearch dfs;

		PetriNetFactory fac;
		fac.addPlace("P1",0,0,0);
		fac.addPlace("P2",0,1,1);
		fac.addTransition("T0",0.5,0.5);
		fac.addInputArc("P1","T0",1);
		fac.addOutputArc("T0","P2",1);

		// Unbounded test
		fac.addTransition("T1",0.5,0.5);
		fac.addInputArc("P2","T1",1);
		fac.addOutputArc("T1","P1",1);

		PetriNet net = *(fac.makePetriNet());

		Mark* m = net.makeEmptyMarking();
		m[0] = 1;

		dfs.reachable(net,m,NULL);
	}

	TEST(DFSReachabilityTest2){
		// Tests another unbounded net
		DepthFirstReachabilitySearch dfs;

		PetriNetFactory fac;

		fac.addPlace("P1",0,0,0);
		fac.addPlace("P2",0,1,1);
		fac.addPlace("P3",0,2,2);

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

		dfs.reachable(net,m,NULL);
	}

	TEST(PetriNetFireTest){

		PetriNetFactory fac;
		fac.addPlace("P1",0,0,0);
		fac.addPlace("P2",0,1,1);
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
