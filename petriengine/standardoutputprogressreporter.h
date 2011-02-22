#ifndef STANDARTOUTPUTPROGRESSREPORTER_H
#define STANDARTOUTPUTPROGRESSREPORTER_H

#include "progressreporter.h"

namespace PetriEngine {

class StandardOutputProgressReporter : public ProgressReporter
{
public:
	StandardOutputProgressReporter() : public ProgressReporter{
		_count = 0;
	}

	void reportProgress(double status);
private:
	int _count;
};
} // PetriEngine
#endif // STANDARTOUTPUTPROGRESSREPORTER_H
