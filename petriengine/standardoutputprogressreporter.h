#ifndef STANDARDOUTPUTPROGRESSREPORTER_H
#define STANDARDOUTPUTPROGRESSREPORTER_H

#include "progressreporter.h"

namespace PetriEngine {

class StandardOutputProgressReporter : public ProgressReporter
{
public:
	StandardOutputProgressReporter() : ProgressReporter(){
		_count = 0;
	}

	void reportProgress(double status);
private:
	int _count;
};
} // PetriEngine
#endif // STANDARDOUTPUTPROGRESSREPORTER_H
