#include "PNMLParser.h"

#include <stdio.h>
#include <PetriEngine/PetriNetBuilder.h>
#include <string>
#include <string.h>

using namespace std;
using namespace PetriEngine;

/* Command line interface:

Mandatory arguments:
--model [pnml-file]						(Model file to load)

Optional arguments, as in either:
--query [query-name]					(Name of the query to execute)
--query-file [sumo-query-file]			(From which query-name will be executed)
Or one of these:
--deadlock								(Perform deadlock check)
--validate								()
Never more than that...

return values:
	0		Successful, query reachable, no-deadlocks or validation successfull
	1		Unsuccesful, query not satisfiable or deadlocks were found
	2		Unknown, algorithm was unable to answer the question
	3		Error, or validation failed, see stderr for messages
 */



int main(int argc, char* argv[]){
	string model;
	string queryname;
	string queryfile;
	bool deadlock = false;
	bool validate = false;

	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], "--model") != 0){
		}else if(strcmp(argv[i], "--model") != 0){
			model = argv[++i];
		}else if(strcmp(argv[i], "--query") != 0){
			queryname = argv[++i];
		}else if(strcmp(argv[i], "--query-file") != 0){
			queryfile = argv[++i];
		}else if(strcmp(argv[i], "--deadlock") != 0){
			deadlock = true;
		}else if(strcmp(argv[i], "--validate") != 0){
			validate = true;
		}
	}

	fprintf(stderr, "running\n");

	return 0;
}
