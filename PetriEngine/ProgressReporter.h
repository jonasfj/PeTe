#ifndef PROGRESSREPORTER_H
#define PROGRESSREPORTER_H

namespace PetriEngine {

/** Reports progress of search */
class ProgressReporter{
public:
	/** Reports progress */
	virtual void reportProgress(double status) = 0;
	/** True, if abortion is requested */
	virtual bool abortRequested() = 0;
};

} // PetriEngine

#endif // PROGRESSREPORTER_H
