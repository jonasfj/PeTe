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
