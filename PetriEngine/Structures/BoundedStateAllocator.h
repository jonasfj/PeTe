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
#ifndef BOUNDEDSTATEALLOCATOR_H
#define BOUNDEDSTATEALLOCATOR_H

#include "../PetriNet.h"
#include "State.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MEMORY_STEP			1024*1024*100

namespace PetriEngine {
namespace Structures {

/** State allocator that is bounded on memory */
template<size_t memory>
class BoundedStateAllocator{
public:
	BoundedStateAllocator(const PetriNet& net){
		_nPlaces = net.numberOfPlaces();
		_nVars = net.numberOfVariables();
		_allocated = memory + MEMORY_STEP;
		do{
			_allocated -= MEMORY_STEP;
			_data = (char*)malloc(_allocated);
		}while(!_data);
		if(!_data)
			fprintf(stderr, "Memory Allocation Failure: Unable to allocated even 100 MiB!\n");
		_allocated = memory;
		_offset = _data;
	}
	~BoundedStateAllocator(){
		if(_data){
			free(_data);
			_data = NULL;
		}
		_offset = NULL;
		_allocated = 0;
	}

	/** Create new state */
	State* createState(){
		if(_offset - _data + stateSize() >= _allocated)
			return NULL;
		char* d = _offset;
		State* s = (State*)d;
		s->_parent = NULL;
		s->_parentTransition = 0;
		s->_marking = (MarkVal*)(d + sizeof(State));
		s->_valuation = (VarVal*)(d + sizeof(State) + sizeof(MarkVal) * _nPlaces);
		_offset += stateSize();
		return s;
	}
private:
	size_t stateSize(){
		return sizeof(State) + sizeof(MarkVal) * _nPlaces + sizeof(VarVal) * _nVars;
	}
	char* _data;
	char* _offset;
	size_t _allocated;
	int _nPlaces;
	int _nVars;
};

} // Structures
} // PetriEngine

#endif // BOUNDEDSTATEALLOCATOR_H
