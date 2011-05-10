#include "PNMLParser.h"

#include <stdio.h>
#include <PetriEngine/PetriNetBuilder.h>
#include <PetriEngine/PQL/PQL.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "PetriEngine/PQL/PQLParser.h"

using namespace std;
using namespace PetriEngine;
using namespace PetriEngine::PQL;

/* Command line interface:

Mandatory arguments:
--model [pnml-file]						(Model file to load)
--query [query-name]					(Name of the query to execute)
--query-file [sumo-query-file]			(From which query-name will be executed)

return values:
	0		Successful, query reachable
	1		Unsuccesful, query not satisfiable
	2		Unknown, algorithm was unable to answer the question
	3		Error, see stderr for messages

Prints to stdout:
	Deadlock:
		DEADLOCKS <DVAL> TECHNIQUES <TVAL1> <TVAL2> ...
			where:
			- <DVAL> is the number of deadlocks in the system,
	Validation:
		FORMULA <NAME1> <RES1> TECHNIQUES <TVAL1> <TVAL2>...
		FORMULA <NAME2> <RES1> TECHNIQUES <TVAL1> <TVAL2>...
			where:
			- <NAME> is the name of the formula in the file,
			- <RES> is TRUE or FALSE,


		- <TVALx> are techniques selected by the tool to perform
			this operation (several may be used together).
			Classification of these techniques are done by means of
			the following keywords:
				- EXPLICIT
				- ABSTRACTIONS
				- PARTIAL_ORDERS
				- PARALLEL_PROCESSING
				- DECISION_DIAGRAMS
				- SYMMETRIES
				- STATE_COMPRESSION
				- OTHERS

	If computation was not completed, must print either
		- CANNOT_COMPUTE
		- DO_NOT_COMPETE

	Nothing else may be printed to stdout.
 */

/** Enumeration of return values from PeTer */
enum ReturnValues{
	SuccessCode	= 0,
	FailedCode	= 1,
	UnknownCode	= 2,
	ErrorCode	= 3
};


int main(int argc, char* argv[]){
	// Commandline arguments
	bool listformulas = false;
	char* model = NULL;
	char* queryname = NULL;
	char* queryfile = NULL;

	// Parse command line arguments
	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], "--model") == 0){
			model = argv[++i];
		}else if(strcmp(argv[i], "--query") == 0){
			queryname = argv[++i];
		}else if(strcmp(argv[i], "--query-file") == 0){
			queryfile = argv[++i];
		}else if(strcmp(argv[i], "--list-queries") == 0){
			listformulas = true;
		}
	}

	// Validate command line arguments
	if(!model && !listformulas){
		fprintf(stderr, "Argument Error: Model must be provided using --model\n");
		return ErrorCode;
	}
	if(!queryname && !listformulas){
		fprintf(stderr, "Argument Error: Query name must be provided using --query\n");
		return ErrorCode;
	}
	if(!queryfile){
		fprintf(stderr, "Argument Error: Query file must be provided using --query-file\n");
		return ErrorCode;
	}

	//----------------------- Queries -----------------------//

	//Condition to check
	Condition* query = NULL;
	bool isInvariant = false;

	//Parse query file, begin scope to release memory from the stack
	{
		map<string, SUMoQuery> queries;

		// Load formulas
		ifstream formulafile(queryfile, ifstream::in);
		if(!formulafile){
			fprintf(stderr, "Argument Error: Formula file \"%s\" couldn't be opened\n", queryfile);
			return ErrorCode;
		}

		// Parse queries into map
		while(!formulafile.eof()){
			string queryString;
			getline(formulafile, queryString);
			if(queryString == "")
				continue;

			//Parse queries
			SUMoQuery q = ParseSUMoQuery(queryString);
			if(!q.query){
				fprintf(stderr, "Argument Error: Failed to parse query \"%s\" in file \"%s\"", queryString.c_str(), queryfile);
				return ErrorCode;
			}

			queries[q.name] = q;
		}

		formulafile.close();

		// List possible formulas, then terminate
		if(listformulas){
			typedef map<string, SUMoQuery>::iterator iter;
			for(iter it = queries.begin(); it != queries.end(); it++){
				fprintf(stdout, "%s\n", (*it).second.name.c_str());
			}
			return SuccessCode;
		}

		// Assign queries and move on
		string formula(queryname);
		query = queries[formula].query;
		isInvariant = queries[formula].isInvariant;
	}

	//----------------------- Models -----------------------//

	//Load the model, begin scope to release memory from the stack
	PetriNet* net = NULL;
	MarkVal* m0 = NULL;
	VarVal* v0 = NULL;
	{
		//Load the model
		ifstream modelfile(model, ifstream::in);
		if(!modelfile){
			fprintf(stderr, "Argument Error: Model file \"%s\" couldn't be opened\n", model);
			return ErrorCode;
		}

		//Read everything
		stringstream buffer;
		buffer << modelfile.rdbuf();

		//Parse and build the petri net
		PetriNetBuilder builder(false);
		PNMLParser parser;
		parser.parse(buffer.str(), &builder);
		parser.makePetriNet();

		//Build the petri net
		net = builder.makePetriNet();
		m0 = builder.makeInitialMarking();
		v0 = builder.makeInitialAssignment();

		// Close the file
		modelfile.close();
	}


	//TODO: Do reachability with the best method we've got

	return UnknownCode;
}






























