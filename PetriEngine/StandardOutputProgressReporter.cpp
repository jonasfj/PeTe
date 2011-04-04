#include "StandardOutputProgressReporter.h"

#include <stdio.h>

namespace PetriEngine {

/** Reports progress to standard output */
void StandardOutputProgressReporter::reportProgress(double status){

	while(_count-- > 0)
		printf("\b");

	if(status < 1)
		_count = printf("Progress: %4.2f%%", status * 100);

	fflush(stdout);

	if(_count < 0)
		_count = 0;

}

bool StandardOutputProgressReporter::abortRequested(){
	return false;
}

} // PetriEngine
