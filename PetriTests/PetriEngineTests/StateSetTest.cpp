#include <UnitTest++.h>
#include "PetriNet.h"
#include "Reachability/State.h"
#include "Reachability/StateSet.h"

using namespace PetriEngine;
using namespace PetriEngine::Reachability;

SUITE(StateSetTest){
	TEST(StateSetDuplicationDetection){
		//The states
		State* s1 = State::createState(5, 5);
		State* s2 = State::createState(5, 5);
		//Assign stuff to states
		/*for(int i = 0; i < 5; i++){
			s1->marking()[i] = i;
			s2->marking()[i] = i;
			s1->valuation()[i] = i;
			s2->valuation()[i] = i;
		}*/
		//Add states to stateSet!
		StateSet set(5, 5);
		CHECK(set.add(s1));
		CHECK(!set.add(s2));

	}
}
