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
#ifndef REACHABILITYRESULT_H
#define REACHABILITYRESULT_H

namespace PetriEngine { namespace Reachability {

// Big int used for state space statistics
typedef unsigned long long int BigInt;

/** Result of a reachability search */
class ReachabilityResult{
public:
	/** Types of results */
	enum Result{
		/** The query was satisfied */
		Satisfied,
		/** The query cannot be satisfied */
		NotSatisfied,
		/** We're unable to say if the query can be satisfied */
		Unknown
	};

	/** Create a new instance of ReachabilityResult */
	ReachabilityResult(Result result = Unknown, const std::string& explanation = "", BigInt expandedStates = 0, BigInt exploredStates = 0, int pathLength = 0){
		_result = result;
		_explanation = explanation;
		_expandedStates = expandedStates;
		_exploredStates = exploredStates;
		_pathLength = pathLength;
	}

	/** Gets a human readable explanation */
	const std::string& explanation() const { return _explanation;}
	/** Gets the formal result */
	Result result() const{ return _result; }
	/** Gets the number of expanded states.
		A state is expanded when it's children have been added to execution stack */
	BigInt expandedStates() const { return _expandedStates; }
	/** Gets the number of explored states.
		A state is explored when it is visited. */
	BigInt exploredStates() const { return _exploredStates; }
	/** Gets the length of the trace path. */
	int pathLength() const { return _pathLength; }
private:
	std::string _explanation;
	Result _result;
	BigInt _expandedStates;
	BigInt _exploredStates;
	int _pathLength;
};

} // Reachability
} // PetriEngine

#endif // REACHABILITYRESULT_H
