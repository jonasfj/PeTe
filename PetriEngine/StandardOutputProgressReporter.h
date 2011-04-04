#ifndef STANDARDOUTPUTPROGRESSREPORTER_H
#define STANDARDOUTPUTPROGRESSREPORTER_H

#include "ProgressReporter.h"

namespace PetriEngine {

/** Progress reporter, reporting to stdout */
class StandardOutputProgressReporter : public ProgressReporter
{
public:
	StandardOutputProgressReporter() : ProgressReporter(){
		_count = 0;
	}

	void reportProgress(double status);
	bool abortRequested();
private:
	int _count;
};

} // PetriEngine
#endif // STANDARDOUTPUTPROGRESSREPORTER_H
