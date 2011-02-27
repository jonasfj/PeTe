#ifndef COVERABILITYTREENODE_H
#define COVERABILITYTREENODE_H

#include <vector>

#include "PetriNet.h"
namespace PetriEngine {

/** Represents a node in the coverability tree of a Petri-net */
class CoverabilityTreeNode
{
public:
	/** Constructs a new node with its parent and a heap-allocated marking. This class takes ownership of marking */
	CoverabilityTreeNode(CoverabilityTreeNode* parent, int transition, Mark* marking);
	CoverabilityTreeNode(Mark* marking);

	/** Adds a child node */
	void add(CoverabilityTreeNode* node);
	/** Checks if the node is new in the coverability tree */
	bool findDuplicate(PetriNet& net);

	// Getters/setters
	bool isDeadEnd(){ return _isDeadEnd; }
	void setDeadEnd(bool value){ _isDeadEnd=value; }

	bool isOld(){ return _isOld; }
	void setOld(bool value){ _isOld=value; }

	// Destructor
	~CoverabilityTreeNode(){
		if(_marking)
			delete _marking;
		// TODO: destructor for child nodes
	}

private:
	/** The parent node, which is null for the root node */
	CoverabilityTreeNode* _parent;
	int _transition;
	Mark* _marking;
	std::vector<CoverabilityTreeNode*> _childNodes;

	bool _isOld;
	bool _isDeadEnd;

};
} // "PetriEngine"
#endif // COVERABILITYTREENODE_H
