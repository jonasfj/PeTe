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
		size_t size = sizeof(SmartState*) + nPlaces * sizeof(MarkVal) + nVars * sizeof(VarVal);
		if(offset - data + size > memory)
			return NULL;
		SmartState* s = (SmartState*)offset;
		s->setParent(NULL);
		s->setTransition(0, 0);
		s->_marking = (MarkVal*)(offset + sizeof(SmartState*));
		s->_valuation = (MarkVal*)(offset + sizeof(SmartState*) + nPlaces * sizeof(MarkVal));
		offset += size;
		return s;
	}
	/** Create new non-stored state, returns NULL if out of memory */
	SmartState* createState(SmartState* parent = NULL, unsigned int transition = 0, int multiplicity = 1){
		size_t size = sizeof(SmartState*);
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
private:
	int nPlaces, nVars;
	char* data;
	char* offset;
};

} // Structures
} // PetriEngine

#endif // SMARTSTATEALLOCATOR_H
