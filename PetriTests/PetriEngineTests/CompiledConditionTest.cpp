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
#include "PQL/PQLParser.h"
#include "PQL/PQL.h"
#include "PQL/Contexts.h"
#include "PQL/CompiledCondition.h"

using namespace PetriEngine;
using namespace PetriEngine::PQL;
using namespace std;

SUITE(CompiledConditionTest){
	TEST(SimpleCompiledConditionTest){
		Condition* cond = ParseQuery("P1 == 10 or P2 == 5");
		vector<string> places;
		vector<string> vars;
		places.push_back("P1");
		places.push_back("P2");
		AnalysisContext a(places, vars);

		CompiledCondition* C = new CompiledCondition(cond);

		C->analyze(a);
		CHECK(a.errors().size() == 0);
		CHECK(C->compile());

		MarkVal m1[2];
		m1[0] = 9;
		m1[1] = 5;
		CHECK(C->evaluate(EvaluationContext(m1, NULL)));
		m1[0] = 9;
		m1[1] = 4;
		CHECK(!C->evaluate(EvaluationContext(m1, NULL)));
		m1[0] = 10;
		m1[1] = 3;
		CHECK(C->evaluate(EvaluationContext(m1, NULL)));
	}
}
