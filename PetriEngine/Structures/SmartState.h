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
