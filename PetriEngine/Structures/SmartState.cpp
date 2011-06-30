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
#include "SmartState.h"

#include <string.h>
#include <algorithm>

namespace PetriEngine{
namespace Structures{

int SmartState::getState(const PetriNet& net, MarkVal* marking, VarVal* valuation) const{
	if(stored()){
		memcpy(marking, _marking, sizeof(MarkVal) * net.numberOfPlaces());
		memcpy(valuation, _valuation, sizeof(VarVal) * net.numberOfVariables());
		return 0;
	}
	int depth = parent()->getState(net, marking, valuation);
	net.fireWithoutCheck(transition(), marking, valuation, marking, valuation, multiplicity());
	return depth + 1;
}

int SmartState::pathLength(){
	if(_parent)
		return multiplicity() + _parent->pathLength();
	else
		return 0;
}

std::vector<unsigned int> SmartState::trace(){
	SmartState* current = this;
	std::vector<unsigned int> trace;
	while(current->parent()){
		for(unsigned int i = 0; i < multiplicity(); i++)
			trace.push_back(current->transition());
		current = current->_parent;
	}
	std::reverse(trace.begin(), trace.end());
	return trace;
}

} // Structures
} // PetriEngine
