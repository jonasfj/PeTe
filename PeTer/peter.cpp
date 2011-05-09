#include <stdio.h>

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
	fprintf(stderr, "Running\n");


	return 0;
}
