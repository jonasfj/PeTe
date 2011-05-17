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
#include "RandomQueryGenerator.h"

#include "../PQL/PQL.h"
#include "../PQL/Contexts.h"
#include "../Structures/StateSet.h"
#include "../Structures/StateAllocator.h"

#include <sstream>
#include <list>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

using namespace std;
using namespace PetriEngine::PQL;
using namespace PetriEngine::Structures;

namespace PetriEngine {
namespace Reachability {

/** Generate a random query */
std::string RandomQueryGenerator::gen(const PetriNet& net,
									  const MarkVal* m0,
									  const VarVal* v0){
	srand(time(0));	// Chosen by fair dice roll

	StateAllocator<> allocator(net);
	State* s = allocator.createState();
	memcpy(s->marking(), m0, sizeof(MarkVal)*net.numberOfPlaces());
	memcpy(s->valuation(), v0, sizeof(VarVal)*net.numberOfVariables());

	int countdown = rand() % 5000000;

	State* ns = allocator.createState();
	while(countdown-- > 0){
		unsigned int  tstart = rand() % net.numberOfTransitions();
		bool done = false;
		for(unsigned int t = tstart; t < net.numberOfTransitions(); t++){
			if(net.fire(t, s, ns)){
				done = true;
				tstart = 0;
				break;
			}
		}
		for(unsigned int t = 0; t < tstart; t++){
			if(net.fire(t, s, ns)){
				done = true;
				break;
			}
		}
		if(!done)
			return "";
		else{
			State* tmp = s;
			s = ns;
			ns = tmp;
		}
	}
	stringstream ss;
	ss<<net.placeNames()[0]<<" == "<<s->marking()[0];
	for(unsigned int p = 1; p < net.numberOfPlaces(); p++)
		ss<<" and "<<net.placeNames()[p]<<" == "<<s->marking()[p];
	for(unsigned int x = 0; x < net.numberOfVariables(); x++)
		ss<<" and "<<net.variableNames()[x]<<" == "<<s->valuation()[x];
	return ss.str();
}

} // Reachability
} // PetriEngine
