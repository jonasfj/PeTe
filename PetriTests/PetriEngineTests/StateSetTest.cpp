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
