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
#ifndef SMARTSTATEALLOCATOR_H
#define SMARTSTATEALLOCATOR_H

#include "../PetriNet.h"
#include "SmartState.h"

#include <stdlib.h>
#include <string.h>

namespace PetriEngine{
namespace Structures{

template<size_t memory>
class SmartStateAllocator{
public:
	SmartStateAllocator(const PetriNet& net){
		data = new char[memory];
		offset = data;
		nPlaces = net.numberOfPlaces();
		nVars = net.numberOfVariables();
	}

	~SmartStateAllocator(){
		if(data){
			delete[] data;
			data = NULL;
		}
	}
	/** Create new state, returns NULL if out of memory */
	SmartState* createStoredState(){
		size_t size = sizeof(SmartState) + nPlaces * sizeof(MarkVal) + nVars * sizeof(VarVal);
		if(offset - data + size > memory)
			return NULL;
		SmartState* s = (SmartState*)offset;
		s->setParent(NULL);
		s->setTransition(0, 0);
		s->_marking = (MarkVal*)(offset + sizeof(SmartState));
		s->_valuation = (VarVal*)(offset + sizeof(SmartState) + nPlaces * sizeof(MarkVal));
		offset += size;
		return s;
	}
	/** Create new non-stored state, returns NULL if out of memory */
	SmartState* createState(SmartState* parent = NULL, unsigned int transition = 0, int multiplicity = 1){
		size_t size = sizeof(SmartState);
		if(offset - data + size > memory)
			return NULL;
		SmartState* s = (SmartState*)offset;
		s->setParent(parent);
		s->setTransition(transition, multiplicity);
		s->_marking = NULL;
		s->_valuation = NULL;
		offset += size;
		return s;
	}
	/** Get memory usage as double between 1 and 100 */
	double percentMemoryUsed() const{
		return ((double)(offset - data)) / (((double)memory)/100);
	}
private:
	int nPlaces, nVars;
	char* data;
	char* offset;
};

} // Structures
} // PetriEngine

#endif // SMARTSTATEALLOCATOR_H
