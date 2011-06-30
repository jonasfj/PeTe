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
#ifndef SMARTSTATE_H
#define SMARTSTATE_H

#include "../PetriNet.h"

namespace PetriEngine{
namespace Structures{

template<size_t memory> class SmartStateAllocator;

/** Smart state, a state that doesn't necessarily need to store everything */
class SmartState{
public:
	/** True, if this states is stored */
	bool stored() const { return _marking && _valuation; }
	/** Number of times transition was fired in parent to get to this */
	int multiplicity() const { return _multiplicity; }
	/** Transition fired in parent to get to this */
	unsigned int transition() const { return _transition; }
	/** Get parent state */
	const SmartState* parent() const { return _parent; }

	/** Computate marking and valuation, if needed
	 * @returns Number states traveled up to find a stored marking
	 */
	int getState(const PetriNet& net, MarkVal* marking, VarVal* valuation) const;

	/** Get trace from initial state */
	std::vector<unsigned int> trace();

	/** Get path length from initial state */
	int pathLength();

	void setTransition(unsigned int transition, int multiplicity = 1) {
		_transition = transition;
		_multiplicity = multiplicity;
	}
	void setParent(SmartState* parent) { _parent = parent; }

	const MarkVal* marking() const { return _marking; };
	const VarVal* valuation() const {return _valuation; };
	MarkVal* marking() { return _marking; };
	VarVal* valuation() {return _valuation; };
private:
	int _multiplicity;
	unsigned int _transition;
	SmartState* _parent;
	MarkVal* _marking;
	VarVal* _valuation;
	template<size_t memory> friend class SmartStateAllocator;
} __attribute__((__packed__));

} // Structures
} // PetriEngine

#endif // SMARTSTATE_H
