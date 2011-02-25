#ifndef COVERABILITYTREENODE_H
#define COVERABILITYTREENODE_H

#include <vector>

#include "PetriNet.h"
namespace PetriEngine {
class CoverabilityTreeNode
{
public:
	CoverabilityTreeNode(int transition, Mark* marking);
	void add(int transition, Mark* marking);
private:
	int _transition;
	Mark* _marking;
	std::vector<CoverabilityTreeNode> _childNodes;
};
} // "PetriEngine"
#endif // COVERABILITYTREENODE_H
