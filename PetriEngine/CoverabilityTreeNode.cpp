#include "CoverabilityTreeNode.h"

namespace PetriEngine {

CoverabilityTreeNode::CoverabilityTreeNode(CoverabilityTreeNode* parent, int transition, Mark* marking)
{
	_parent = parent;
	_transition = transition;
	_marking = marking;
	_isDeadEnd = false;
	_isOld = false;
}

CoverabilityTreeNode::CoverabilityTreeNode(Mark *marking){
	_marking = marking;
	_isDeadEnd = false;
	_isOld = false;
	_parent = NULL;
}

/** Adds a new marking to the set of child nodes */
void CoverabilityTreeNode::add(CoverabilityTreeNode node){
	_childNodes.push_back(node);
}

bool CoverabilityTreeNode::findDuplicate(PetriNet& net){
	bool duplicateFound = false;

	CoverabilityTreeNode* currentParent = _parent;
	// While there is a parent on the path
	while(currentParent){

		// Compare the marking of some parent and M
		bool areEqual = false;
		for(int i = 0; i < net.nTransitions(); i++){
			if(this->_marking[i] != _parent->_marking[i]){
				areEqual = false;
				break;
			}
			areEqual = true;
		}
		duplicateFound = areEqual;

		// If we found a duplicate, no need to search further
		if(duplicateFound) {
			break;
		} else if(currentParent->_parent){
			currentParent = currentParent->_parent;
		}
	}

	return duplicateFound;
}

} // PetriEngine
