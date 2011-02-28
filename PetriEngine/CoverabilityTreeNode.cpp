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
void CoverabilityTreeNode::add(CoverabilityTreeNode* node){
	_childNodes.push_back(node);
}

bool markingEqual(Mark* m1, Mark* m2, PetriNet net){
	bool areEqual = false;
	for(int i = 0; i < net.nPlaces(); i++){
		if(m1[i] != m2[i]){
			areEqual = false;
			break;
		}
		areEqual = true;
	}
	return areEqual;
}

bool markingGreaterThanOrEqual(Mark* m1, Mark* m2, PetriNet net){
	bool allGEQ = false;
	for(int i = 0; i < net.nPlaces(); i++){
		if(m1[i] < m2[i]){
			allGEQ = false;
			break;
		}
		allGEQ = true;
	}
	return allGEQ;
}

bool CoverabilityTreeNode::findDuplicate(PetriNet& net){
	bool foundAnyDuplicates = false;

	CoverabilityTreeNode* currentParent = this->_parent;
	while(currentParent){

		// Compare the marking of some parent and M
		bool isDuplicate = markingEqual(this->_marking,currentParent->_marking,net);
		if(isDuplicate)
			foundAnyDuplicates = true;

		// If the are not equal, check for inifinity
		if(!isDuplicate) {
			// Make sure all markings of m1 are greater than or equal to m2.
			bool geq = markingGreaterThanOrEqual(this->_marking,currentParent->_marking,net);

			// Find places with strictly more tokens and replace with inifinity
			if(geq){
				for(int i = 0; i< net.nPlaces(); i++){
					if(this->_marking[i] > currentParent->_marking[i]){
						// Replace m[i] with infinity
						this->_marking[i] = MARK_INF;
					}
				}
			}
		}
			// Go one marking up in the path
			if(currentParent->_parent)
				currentParent = currentParent->_parent;
			else
				currentParent = NULL;
		}
	return foundAnyDuplicates;
}

} // PetriEngine
