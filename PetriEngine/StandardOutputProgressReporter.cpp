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
