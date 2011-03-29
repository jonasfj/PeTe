#ifndef COVERABILITYTREENODE_H
#define COVERABILITYTREENODE_H

#include <vector>

#include "PetriNet.h"
namespace PetriEngine { namespace Reachability {

/** Represents a node in the coverability tree of a Petri-net */
class CoverabilityTreeNode
{
public:
	/** Constructs a new node with its parent and a heap-allocated marking. This class takes ownership of marking */
	CoverabilityTreeNode(CoverabilityTreeNode* parent, int transition, MarkVal* marking, VarVal* assignments);
	CoverabilityTreeNode(MarkVal* marking, VarVal* assignments);

	/** Adds a child node */
	void add(CoverabilityTreeNode* node);
	/** Checks if the node is new in the coverability tree */
	bool findDuplicate(const PetriNet& net);

	// Getters/setters
	bool isDeadEnd(){ return _isDeadEnd; }
	void setDeadEnd(bool value){ _isDeadEnd=value; }

	bool isOld(){ return _isOld; }
	void setOld(bool value){ _isOld=value; }

	// TODO: Fix this some day, on the horizon
	// Destructor
	/*
	~CoverabilityTreeNode(){
		_parent = NULL;
		if(_childNodes.size() != 0){
			for(unsigned int i = 0; i < _childNodes.size(); i++){
				if(_childNodes[i])
					delete _childNodes[i];
				_childNodes[i] = NULL;
			}
		}
	}*/

	const std::vector<CoverabilityTreeNode*> childNodes() const{
		return _childNodes;
	}
	 MarkVal* marking() const{
		return _marking;
	}
	 int transition() const{
		return _transition;
	}

private:
	/** The parent node, which is null for the root node */
	CoverabilityTreeNode* _parent;
	int _transition;
	MarkVal* _marking;
	VarVal* _assignments;
	std::vector<CoverabilityTreeNode*> _childNodes;

	bool _isOld;
	bool _isDeadEnd;

};
} // "Reachability"
} // "PetriEngine"
#endif // COVERABILITYTREENODE_H
