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
#ifndef REACHABILITYSEARCHSTRATEGY_H
#define REACHABILITYSEARCHSTRATEGY_H

#include "../PetriNet.h"
#include "../ProgressReporter.h"
#include "../PQL/PQLParser.h"

#include <string>
#include <vector>

#include "ReachabilityResult.h"

namespace PetriEngine {

namespace PQL{
	class Condition;
}

namespace Reachability{

/** Represents an abstract reachability search strategy. */
class ReachabilitySearchStrategy {
public:
	ReachabilitySearchStrategy(){
		_reporter = NULL;
	}

	/** Determines if a petrinet is reachable w.r.t. a query */
	virtual ReachabilityResult reachable(const PetriNet &net,
										 const MarkVal* initialMarking,
										 const VarVal* initialAssignment,
										 PQL::Condition* query) = 0;

	/** Sets the concrete progress reporter */
	void setProgressReporter(ProgressReporter* reporter) {
		_reporter = reporter;
	}

	/** List all reachability strategies, return unqiue display names */
	static std::vector<std::string> listStrategies();

	/** Create a reachability strategy from string, NULL if not found */
	static ReachabilitySearchStrategy* createStrategy(const std::string& strategy);

	/** Get the progress reporter, used for nesting strategies */
	ProgressReporter* reporter() {return _reporter; }
protected:
	/** Reports the progress of reachability search (number between 0 and 1) */
	void reportProgress(double status){
		if(_reporter)
			_reporter->reportProgress(status);
	}

	/** Returns, true if abortion have been requested */
	bool abortRequested(){
		if(_reporter)
			return _reporter->abortRequested();
		return false;
	}
private:
	ProgressReporter* _reporter;
};


} // Reachability
} // PetriEngine

#endif // REACHABILITYSEARCHSTRATEGY_H
