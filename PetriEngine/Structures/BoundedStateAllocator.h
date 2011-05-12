#ifndef BOUNDEDSTATEALLOCATOR_H
#define BOUNDEDSTATEALLOCATOR_H

#include "../PetriNet.h"
#include "State.h"

#include <stdlib.h>
#include <string.h>


namespace PetriEngine {
namespace Structures {

/** State allocator that is bounded on memory */
template<size_t memory>
class BoundedStateAllocator{
public:
	BoundedStateAllocator(const PetriNet& net){
		_nPlaces = net.numberOfPlaces();
		_nVars = net.numberOfVariables();
		_data = new char[memory];
		_offset = _data;
	}
	~BoundedStateAllocator(){
		if(_data){
			delete[] _data;
			_data = NULL;
		}
		_offset = NULL;
	}

	/** Create new state */
	State* createState(){
		if(_offset - _data + stateSize() >= memory)
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
	int _nPlaces;
	int _nVars;
};

} // Structures
} // PetriEngine

#endif // BOUNDEDSTATEALLOCATOR_H
