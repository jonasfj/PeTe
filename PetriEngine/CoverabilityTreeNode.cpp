#include "CoverabilityTreeNode.h"
#include <stdio.h>
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

/** Checks if two markings are equal */
bool markingEqual(Mark* m1, Mark* m2, PetriNet net){
	for(int i = 0; i < net.nPlaces(); i++){
		if(m1[i] != m2[i])
			return false;
	}
	return true;
}

/** Checks if marking m1 is greater than or equal to marking m2 */
bool markingGreaterThanOrEqual(Mark* m1, Mark* m2, PetriNet net){
	for(int i = 0; i < net.nPlaces(); i++){
		if(m1[i] < m2[i])
			return false;
	}
	return true;
}

/** Searches the ancestor path for duplicates and coverable nodes */
bool CoverabilityTreeNode::findDuplicate(PetriNet& net){

	//TODO: Could probably done more efficiently with e.g. hashmap

	bool foundAnyDuplicates = false;

	CoverabilityTreeNode* ancestorNode = this->_parent;
	while(ancestorNode != NULL){

		bool markingsAreEqual = markingEqual(this->_marking,ancestorNode->_marking,net);
		if(markingsAreEqual)
			foundAnyDuplicates = true;

		if(!markingsAreEqual) {

			bool allGreaterThanOrEqual = markingGreaterThanOrEqual(this->_marking, ancestorNode->_marking,net);

			// Find places with strictly more tokens and replace with inifinity
			if(allGreaterThanOrEqual){
				for(int i = 0; i < net.nPlaces(); i++){
					if(this->_marking[i] > ancestorNode->_marking[i]){
						// Replace m[i] with infinity
						this->_marking[i] = MARK_INF;
						}
					}
				}
			}
			// Go one marking up in the path
			if(ancestorNode->_parent)
				ancestorNode = ancestorNode->_parent;
			else
				ancestorNode = NULL;
		}
	return foundAnyDuplicates;
}

} // PetriEngine
