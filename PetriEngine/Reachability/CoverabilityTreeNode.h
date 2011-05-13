/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef COVERABILITYTREENODE_H
#define COVERABILITYTREENODE_H

#include <vector>

#include "../PetriNet.h"
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
