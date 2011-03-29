#ifndef REACHABILITYSEARCHSTRATEGY_H
#define REACHABILITYSEARCHSTRATEGY_H

#include "PetriNet.h"
#include "ProgressReporter.h"

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
	virtual bool reachable(const PetriNet &net,
						   const MarkVal* initialMarking,
						   PQL::Condition* query) = 0;

	/** Sets the concrete progress reporter */
	void setProgressReporter(ProgressReporter* reporter) {
		_reporter = reporter;
	}

protected:
	/** Reports the progress of reachability search */
	void reportProgress(double status){
		if(_reporter)
			_reporter->reportProgress(status);
	}

private:
	ProgressReporter* _reporter;
};

} // Reachability

} // PetriEngine

#endif // REACHABILITYSEARCHSTRATEGY_H
