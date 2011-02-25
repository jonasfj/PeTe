#ifndef COVERABILITYTREENODE_H
#define COVERABILITYTREENODE_H

#include <vector>

#include "PetriNet.h"
namespace PetriEngine {
class CoverabilityTreeNode
{
public:
	CoverabilityTreeNode(CoverabilityTreeNode* parent, int transition, Mark* marking);
	CoverabilityTreeNode(Mark* marking);

	/** Adds a child node */
	void add(CoverabilityTreeNode node);

	bool isOld();
	void setOld(bool value);
	bool isDeadEnd();
	void setDeadEnd(bool value);
private:
	CoverabilityTreeNode* _parent;
	int _transition;
	Mark* _marking;
	std::vector<CoverabilityTreeNode> _childNodes;

	bool _isOld;
	bool _isDeadEnd;

};
} // "PetriEngine"
#endif // COVERABILITYTREENODE_H
