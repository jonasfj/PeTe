#include "CoverabilityTreeNode.h"

namespace PetriEngine {

CoverabilityTreeNode::CoverabilityTreeNode(int transition, Mark* marking)
{
	_transition = transition;
	_marking = marking;
}

/** Adds a new marking to the set of child nodes */
void CoverabilityTreeNode::add(int transition, Mark *marking){
	_childNodes.push_back(new CoverabilityTreeNode(transition,marking));
}

} // PetriEngine
