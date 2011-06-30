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
#ifndef STATEALLOCATOR_H
#define STATEALLOCATOR_H

#include "../PetriNet.h"
#include "State.h"

#include <stdlib.h>
#include <string.h>


namespace PetriEngine {
namespace Structures {

/** Class for allocating states in blocks */
template<size_t blocksize = 500000>
class StateAllocator{
	struct Block{
		Block* parent;
		char* m;
	} __attribute__((__packed__));
public:
	StateAllocator(int nPlaces, int nVars){
		_nPlaces = nPlaces;
		_nVars = nVars;
		_b = NULL;
		createNewBlock();
	}
	StateAllocator(const PetriNet& net){
		_nPlaces = net.numberOfPlaces();
		_nVars = net.numberOfVariables();
		_b = NULL;
		createNewBlock();
	}
	~StateAllocator(){
		while(_b){
			Block* nb = _b->parent;
			delete[] (char*)_b;
			_b = nb;
		}
	}
	/** Create new state */
	State* createState(){
		if(_offset == blocksize-1)
			createNewBlock();
		char* d = (_b->m + sizeof(Block) + stateSize() * _offset);
		State* s = (State*)d;
		s->_parent = NULL;
		s->_parentTransition = 0;
		s->_transitionMultiplicity = 0;
		s->_marking = (MarkVal*)(d + sizeof(State));
		s->_valuation = (VarVal*)(d + sizeof(State) + sizeof(MarkVal) * _nPlaces);
		_offset++;
		return s;
	}
	/** Get the size of a state, use for alloca */
	static inline size_t stateSize(const PetriNet& net){
		return sizeof(State) + sizeof(MarkVal) * net.numberOfPlaces() + sizeof(VarVal) * net.numberOfVariables();
	}
	/** Initialize state allocated using alloc, size of memory is expected to be stateSize(net) */
	static inline void initializeState(State* memory, const PetriNet& net){
		char* d = (char*)memory;
		memory->_parent = NULL;
		memory->_parentTransition = 0;
		memory->_transitionMultiplicity = 0;
		memory->_marking = (MarkVal*)(d + sizeof(State));
		memory->_valuation = (VarVal*)(d + sizeof(State) + sizeof(MarkVal) * net.numberOfPlaces());
	}
private:
	size_t stateSize(){
		return sizeof(State) + sizeof(MarkVal) * _nPlaces + sizeof(VarVal) * _nVars;
	}
	void createNewBlock(){
		size_t s = sizeof(Block) + stateSize() * blocksize;
		char* m = new char[s];
		memset(m, 0, s);
		Block* b = (Block*)m;
		b->parent = _b;
		b->m = m + sizeof(Block);
		_b = b;
		_offset = 0;
	}
	size_t _offset;
	Block* _b;
	int _nPlaces;
	int _nVars;
};

} // Structures
} // PetriEngine

#endif // STATEALLOCATOR_H
