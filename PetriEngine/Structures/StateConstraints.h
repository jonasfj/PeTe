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
#ifndef STATECONSTRAINTS_H
#define STATECONSTRAINTS_H

#include "../PetriNet.h"
#include "BitField.h"

#include <vector>

namespace PetriEngine{
namespace Structures{

#define CONSTRAINT_INFTY	1<<20

/** A set of linear constraits that a state can satisfy */
class StateConstraints
{
	/** Constraint on a place or variable */
	struct Constraint{
		int min, max;
	};
public:
	StateConstraints(size_t nPlaces, size_t nVars){
		this->nPlaces = nPlaces;
		this->nVars = nVars;
		pcs = new Constraint[nPlaces];
		vcs = new Constraint[nVars];
		reset();
	}
	StateConstraints(const PetriNet& net){
		nPlaces = net.numberOfPlaces();
		nVars = net.numberOfVariables();
		pcs = new Constraint[nPlaces];
		vcs = new Constraint[nVars];
		reset();
	}
	~StateConstraints(){
		if(pcs){
			delete[] pcs;
			pcs = NULL;
		}
		if(vcs){
			delete[] vcs;
			vcs = NULL;
		}
	}
	/** Set place maximum, returns false if this causes a conflict */
	bool setPlaceMax(int place, MarkVal max){
		if(pcs[place].min > max)
			return false;
		if(max < pcs[place].max)
			pcs[place].max = max;
		return true;
	}
	/** Set place maximum, returns false if this causes a conflict */
	bool setPlaceMin(int place, MarkVal min){
		if(pcs[place].max < min)
			return false;
		if(min > pcs[place].min)
			pcs[place].min = min;
		return true;
	}
	/** Set place maximum, returns false if this causes a conflict */
	bool setVarMax(int var, MarkVal max){
		if(vcs[var].min > max)
			return false;
		if(max < vcs[var].max)
			vcs[var].max = max;
		return true;
	}
	/** Set place maximum, returns false if this causes a conflict */
	bool setVarMin(int var, MarkVal min){
		if(vcs[var].max < min)
			return false;
		if(min > vcs[var].min)
			vcs[var].min = min;
		return true;
	}
	/** Attempts to merge two StateConstraints, return NULL if can't be merged */
	StateConstraints* merge(const StateConstraints* constraint) const;
	/** True, if every marking satisfied by subset is also satisfied by this */
	bool isSuperSet(const StateConstraints* subset) const;
	bool isEqual(const StateConstraints* other) const{
		return this->isSuperSet(other) && other->isSuperSet(this);
	}
	/** Reset all constraints */
	void reset();

	/** Attempts to solve using lp_solve, returns True if the net cannot satisfy these constraints! */
	bool isImpossible(const PetriNet& net, const MarkVal* marking, const VarVal* valuation) const;

	/** Merge the two sets of StateConstraints such that one from A and one from B is always satisfied, when one in the return value is
	 * @remarks This will take ownership of the provided StateConstraints, and delete them or own them
	 */
	static std::vector<StateConstraints*> mergeAnd(const std::vector<StateConstraints*>& A, const std::vector<StateConstraints*>& B);
	/** Merge the two sets of StateConstraints such that either on from A or B is always satisfied, when one in the return value is
	 * @remarks This will take ownership of the provided StateConstraints, and delete them or own them
	 */
	static std::vector<StateConstraints*> mergeOr(const std::vector<StateConstraints*>& A, const std::vector<StateConstraints*>& B);

	static StateConstraints* requirePlaceMin(const PetriNet& net, int place, MarkVal value);
	static StateConstraints* requirePlaceMax(const PetriNet& net, int place, MarkVal value);
	static StateConstraints* requirePlaceEqual(const PetriNet& net, int place, MarkVal value);
	static StateConstraints* requireVarMin(const PetriNet& net, int var, MarkVal value);
	static StateConstraints* requireVarMax(const PetriNet& net, int var, MarkVal value);
	static StateConstraints* requireVarEqual(const PetriNet& net, int var, MarkVal value);
private:
	size_t nPlaces, nVars;
	Constraint* pcs;
	Constraint* vcs;

	BitField maxTrap(const PetriNet& net, BitField places, const MarkVal* resultMarking) const;
	bool isInMaxTrap(const PetriNet& net, size_t place, const BitField& places, const MarkVal* resultMarking) const;
	BitField minimalTrap(const PetriNet& net, const MarkVal* marking, const MarkVal* resultMarking) const;
	bool isMarked(const BitField& places, const MarkVal* marking) const;
};

} // Structures
} // PetriEngine

#endif // STATECONSTRAINTS_H
