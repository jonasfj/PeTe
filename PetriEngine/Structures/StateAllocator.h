#ifndef STATEALLOCATOR_H
#define STATEALLOCATOR_H

#include "../PetriNet.h"
#include "State.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace PetriEngine {
namespace Structures {

/** Class for allocating states in blocks */
template<size_t blocksize>
class StateAllocator{
	struct Block{
		void cleanup(){
			if(parent){
				parent->cleanup();
				free(parent);
			}
			parent = NULL;
		}
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
		if(_b){
			_b->cleanup();
			free(_b);
		}
		_b = NULL;
	}
	/** Create new state */
	State* createState(){
		if(_offset == blocksize)
			createNewBlock();
		char* d = (_b->m + sizeof(Block) + stateSize() * _offset);
		State* s = (State*)d;
		s->_parent = NULL;
		s->_parentTransition = 0;
		s->_marking = (MarkVal*)(d + sizeof(State));
		s->_valuation = (VarVal*)(d + sizeof(State) + sizeof(MarkVal) * _nPlaces);
		_offset++;
		return s;
	}
private:
	size_t stateSize(){
		return sizeof(State) + sizeof(MarkVal) * _nPlaces + sizeof(VarVal) * _nVars;
	}
	void createNewBlock(){
		size_t s = sizeof(Block) + stateSize() * blocksize;
		fprintf(stderr, "size: %i\n", s);
		char* m = (char*)malloc(s);
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
