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
