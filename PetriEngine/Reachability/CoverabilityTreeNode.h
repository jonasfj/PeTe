#ifndef COVERABILITYTREENODE_H
#define COVERABILITYTREENODE_H

#include <vector>

#include "PetriNet.h"
namespace PetriEngine { namespace Reachability {

/** Represents a node in the coverability tree of a Petri-net */
class CoverabilityTreeNode
{
public:
	/** Constructs a new node with its parent and a heap-allocated marking and variable assignment.
	  * This class takes ownership of marking and variable assignment*/
	CoverabilityTreeNode(CoverabilityTreeNode* parent, int transition, MarkVal* marking, VarVal* assignments);
	/** Constructs a root node, and a heap-allocated marking and variable assignment.
	  * This class takes ownership of the marking and variable assignment. */
	CoverabilityTreeNode(MarkVal* marking, VarVal* assignments);

	/** Destroy the CoverabilityTreeNode */
	~CoverabilityTreeNode(){
		_parent = NULL;
		if(_marking)
			delete _marking;
		_marking = NULL;
		if(_assignments)
			delete _assignments;
		_assignments = NULL;
	}

	/** Checks if the node is new in the coverability tree */
	bool findDuplicate(const PetriNet& net);

	/** Getter for marking */
	MarkVal* marking() const{
		return _marking;
	}
	/** Getter for the transition */
	 int transition() const{
		return _transition;
	}

private:
	/** The parent node, which is null for the root node */
	CoverabilityTreeNode* _parent;
	/** The transition being fired */
	int _transition;
	/** The current marking */
	MarkVal* _marking;
	/** The current variable assignments */
	VarVal* _assignments;
};
} // "Reachability"
} // "PetriEngine"
#endif // COVERABILITYTREENODE_H
