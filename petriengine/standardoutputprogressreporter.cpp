#include "standartoutputprogressreporter.h"

#include <stdio.h>

namespace PetriEngine {

void StandardOutputProgressReporter::reportProgress(double status){

	while(_count--)
		printf("\b");

	printf("Status: %4.2d%%", status * 100);
}

} // PetriEngine
