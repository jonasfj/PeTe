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

