#ifndef REACHABILITYSEARCHSTRATEGY_H
#define REACHABILITYSEARCHSTRATEGY_H

#include "petrinet.h"
#include "progressreporter.h"

namespace PetriEngine {

/** Represents an abstract reachability search strategy. */
class ReachabilitySearchStrategy {
public:

	ReachabilitySearchStrategy(){
		_reporter = NULL;
	}

	/** Determines if a petrinet is reachable w.r.t. a query */
	virtual bool reachable(PetriNet net, Mark* initialMarking) = 0;

	/** Sets the concrete progress reporter */
	virtual bool setProgressReporter(ProgressReporter* reporter) = 0;

protected:
	/** Reports the progress of reachability search */
	void reportProgress(double status){
		if(_reporter)
			_reporter->reportProgress(status);
	}

private:
	ProgressReporter* _reporter;
};

} // PetriEngine

#endif // REACHABILITYSEARCHSTRATEGY_H
