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
#ifndef PETRINET_H
#define PETRINET_H

#include <string>
#include <vector>
#include <climits>

namespace PetriEngine{

namespace PQL{
	class Condition;
	class AssignmentExpression;
}

namespace Structures{
	class State;
}

class PetriNetBuilder;

/** Type used for holding markings values */
typedef int MarkVal;
/** Type used for holding variable values */
typedef int VarVal;

#define MARK_INF					INT_MAX

/** Efficient representation of PetriNet */
class PetriNet
{
	PetriNet(int places, int transitions, int variables);
public:
	~PetriNet();
	/** Fire transition if possible and store result in result */
	bool fire(unsigned int transition,
			  const MarkVal* marking,
			  const VarVal* assignment,
			  MarkVal* resultMarking,
			  VarVal* resultAssignment) const;
	bool fire(unsigned int transition, const Structures::State* s, Structures::State* ns, int multiplicity = 1) const;
	/** Fire without checkings conditions */
	void fireWithoutCheck(unsigned int transition,
						  const MarkVal* marking,
						  const VarVal* assignment,
						  MarkVal* resultMarking,
						  VarVal* resultAssignment,
						  int multiplicity = 1) const;
	/** Fire transition if possible and store result in result (Respect MARK_INF */
	bool fireWithMarkInf(unsigned int transition,
			  const MarkVal* marking,
			  const VarVal* assignment,
			  MarkVal* resultMarking,
			  VarVal* resultAssignment) const;
	unsigned int numberOfTransitions() const {return _nTransitions;}
	unsigned int numberOfVariables() const {return _nVariables;}
	unsigned int numberOfPlaces() const {return _nPlaces;}
	int inArc(unsigned int place, unsigned int transition) const;
	int outArc(unsigned int transition, unsigned int place) const;
	/** Get vector place names, don't use this to get the number of places */
	const std::vector<std::string>& placeNames() const {return _places;}
	/** Get vector variable names, don't use this to get the number of variable */
	const std::vector<std::string>& variableNames() const {return _variables;}
	/** Get vector transition names, don't use this to get the number of variable */
	const std::vector<std::string>& transitionNames() const {return _transitions;}
private:
	std::vector<std::string> _places;
	std::vector<std::string> _transitions;
	std::vector<std::string> _variables;
	/** Number of x variables
	 * @remarks We could also get this from the _places vector, but I don't see any
	 * any complexity garentees for this type.
	 */
	size_t _nPlaces, _nTransitions, _nVariables;
	/** Transition matrix, see transition vector */
	MarkVal* _tm;
	/** Get a transition vector
	 * @remarks On form [p1-][p2-]...[p1+][p2+]...
	 *			where p1- is number of tokens to be taken from p1.
	 *			and p1+ is the number of tokens to given to p1.
	 */
	const MarkVal* _tv(unsigned int t) const {return _tm + t * _nPlaces * 2;}
	MarkVal* _tv(unsigned int t) {return _tm + t * _nPlaces * 2;}

	VarVal* _ranges;
	PQL::Condition** _conditions;
	PQL::AssignmentExpression** _assignments;
	friend class PetriNetBuilder;
};

} // PetriEngine

#endif // PETRINET_H
