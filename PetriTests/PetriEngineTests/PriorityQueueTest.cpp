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
#include "Structures/PriorityQueue.h"

using namespace PetriEngine;
using namespace PetriEngine::Structures;

struct MyItem{
	int i;
	int j;
};

SUITE(PriorityQueueTest){
	TEST(SimplePriorityQueueTest){
		MyItem item;
		item.i = 2;
		item.j = 3;

		PriorityQueue<MyItem> queue;
		queue.push(9, item);
		queue.push(2, item);
		item.i = 10;
		queue.push(1, item);
		CHECK(queue.size() == 3);
		CHECK(queue.pop().i == 10);
		CHECK(queue.pop().i == 2);
		CHECK(queue.size() == 1);
	}
}

