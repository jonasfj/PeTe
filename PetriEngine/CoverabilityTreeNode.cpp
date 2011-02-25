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

bool CoverabilityTreeNode::isDeadEnd(){
	return _isDeadEnd;
}
void CoverabilityTreeNode::setDeadEnd(bool value){
	_isDeadEnd=value;
}

bool CoverabilityTreeNode::isOld(){
	return _isOld;
}
void CoverabilityTreeNode::setOld(bool value){
	_isOld=value;
}

} // PetriEngine
