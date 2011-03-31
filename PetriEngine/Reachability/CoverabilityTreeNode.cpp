#include "CoverabilityTreeNode.h"
#include <stdio.h>
namespace PetriEngine { namespace Reachability {

CoverabilityTreeNode::CoverabilityTreeNode(CoverabilityTreeNode* parent, int transition, MarkVal* marking, VarVal* assignments){
	_parent = parent;
	_transition = transition;
	_marking = marking;
	_assignments = assignments;
}

CoverabilityTreeNode::CoverabilityTreeNode(MarkVal *marking, VarVal* assignments){
	_marking = marking;
	_assignments = assignments;
	_parent = NULL;
}

/** Searches the ancestor path for duplicates and coverable nodes */
bool CoverabilityTreeNode::findDuplicate(const PetriNet& net){
	//TODO: Could probably done more efficiently with e.g. hashmap
	bool foundAnyDuplicates = true;
	CoverabilityTreeNode* ancestorNode = this->_parent;

	if(!ancestorNode)
		return false;

	while(ancestorNode){
		for(unsigned int i = 0; i < net.numberOfPlaces(); i++){
			// Check assignments
			if(_assignments[i] != ancestorNode->_assignments[i])
				foundAnyDuplicates = false;

			// Check marking
			if(_marking[i] != ancestorNode->_marking[i]){
				foundAnyDuplicates = false;
				// Replace m[i] with infinity on strictly larger marks
				//Uncomment for unbounded
				//if(_marking[i] > ancestorNode->_marking[i])
					//_marking[i] = MARK_INF;
			}
		}

		// While loop maintenance
		if(!foundAnyDuplicates){
			if(ancestorNode->_parent)
				ancestorNode = ancestorNode->_parent;
			else
				ancestorNode = NULL;
		}
	}

	return foundAnyDuplicates;
}

} // Reachability
} // PetriEngine
