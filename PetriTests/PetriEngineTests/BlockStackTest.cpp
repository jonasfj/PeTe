#include <UnitTest++.h>
#include "PetriNet.h"
#include "Structures/BlockStack.h"

using namespace PetriEngine;
using namespace PetriEngine::Structures;

SUITE(BlockStackTest){
	TEST(SimpleBlockStackTest){
		BlockStack<int, 3> stack;
		stack.push(1);
		stack.push(2);
		stack.push(3);
		stack.push(4);
		CHECK(stack.pop());
		CHECK(*stack.top() == 3);
		CHECK(stack.pop());
		CHECK(*stack.top() == 2);
		CHECK(stack.pop());
		CHECK(*stack.top() == 1);
		CHECK(stack.pop());
		CHECK(stack.top() == NULL);
		CHECK(!stack.pop());

		stack.push(1);
		stack.push(2);
		stack.push(3);
		stack.push(4);
		CHECK(stack.pop());
		CHECK(*stack.top() == 3);
		CHECK(stack.pop());
		CHECK(*stack.top() == 2);
		CHECK(stack.pop());
		CHECK(*stack.top() == 1);
		CHECK(stack.pop());
		CHECK(stack.top() == NULL);
		CHECK(!stack.pop());

		stack.push(1);
		stack.push(2);
		stack.push(3);
		stack.push(4);
		stack.push(1);
		stack.push(2);
		stack.push(3);
		stack.push(4);
		CHECK(stack.pop());
		CHECK(*stack.top() == 3);
		CHECK(stack.pop());
		CHECK(*stack.top() == 2);
		CHECK(stack.pop());
		CHECK(*stack.top() == 1);
		CHECK(stack.pop());
		CHECK(*stack.top() == 4);
		CHECK(stack.pop());
		CHECK(*stack.top() == 3);
		CHECK(stack.pop());
		CHECK(*stack.top() == 2);
		CHECK(stack.pop());
		CHECK(*stack.top() == 1);
		CHECK(stack.pop());
		CHECK(stack.top() == NULL);
		CHECK(!stack.pop());
	}
}
