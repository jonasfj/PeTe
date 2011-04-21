#include <UnitTest++.h>
#include "PetriNet.h"
#include "Structures/State.h"
#include "Structures/StateSet.h"
#include "Structures/StateAllocator.h"


using namespace PetriEngine;
using namespace PetriEngine::Structures;

SUITE(StateSetTest){
	TEST(StateSetDuplicationDetection){
		StateAllocator<10> allocator(5,5);
		//The states
		State* s1 = allocator.createState();
		State* s2 = allocator.createState();
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
