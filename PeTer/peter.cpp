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
#include "PetriEngine/Reachability/LinearOverApprox.h"
#include "PetriEngine/Reachability/BestFirstReachabilitySearch.h"
#include "PetriEngine/PQL/Contexts.h"
#include "PetriEngine/Reachability/RandomDFS.h"
#include "PetriEngine/Reachability/RandomQueryGenerator.h"

using namespace std;
using namespace PetriEngine;
using namespace PetriEngine::PQL;
using namespace PetriEngine::Reachability;

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

int test(int argc, char *argv[]);

int main(int argc, char* argv[]){
	// Test procedure
	if(argc > 1 && strcmp(argv[1], "--test") == 0)
		return test(argc-1, argv+1);

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

		// Assign queries and invariant
		string formula(queryname);
		query = queries[formula].query;
		isInvariant = queries[formula].isInvariant;

		//Cleanup
		typedef map<string, SUMoQuery>::iterator iter;
		for(iter it = queries.begin(); it != queries.end(); it++){
			if((*it).first != formula)
				delete (*it).second.query;
		}
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

	//----------------------- Reachability -----------------------//


	//Begin reachability search, begin scope to release memory from the stack
	{
		//Fallback strategy
		int flags = DistanceContext::AndSum | DistanceContext::OrExtreme;
		BestFirstReachabilitySearch fallback((DistanceContext::DistanceStrategy)flags);
		//Linear Approximation
		LinearOverApprox strategy(&fallback);

		//Context analysis
		{
			AnalysisContext context(*net);
			query->analyze(context);

			for(size_t i = 0; i < context.errors().size(); i++){
				fprintf(stderr, "Query Context Analysis Error: %s\n", context.errors()[i].toString().c_str());
			}

			if(context.errors().size() > 0)
				return ErrorCode;
		}

		ReachabilityResult result = strategy.reachable(*net, m0, v0, query);

		//TODO: Double check technique print.. I don't really know what to put here
		string techniques("EXPLICIT HEURISTIC");

		if(result.result() == ReachabilityResult::Satisfied){
			string resultText = isInvariant ? "FALSE" : "TRUE";
			fprintf(stdout, "FORMULA %s %s %s\n", queryname, resultText.c_str(), techniques.c_str());
			return FailedCode;
		}else if(result.result() == ReachabilityResult::NotSatisfied){
			string resultText = isInvariant ? "TRUE" : "FALSE";
			fprintf(stdout, "FORMULA %s %s %s\n", queryname, resultText.c_str(), techniques.c_str());
			return SuccessCode;
		} else {
			//Unknown. Could not compute.
			fprintf(stdout, "CANNOT_COMPUTE\n");
			return UnknownCode;
		}
	}

	return UnknownCode;
}

/** Procedure for testing PeTe without Qt dependencies */
int test(int argc, char* argv[]){

	bool genRandState = false;
	bool listformulas = false;
	string queryname,
		   strategy,
		   filename,
		   queryString;

	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], "--query") == 0){
			queryname = argv[++i];
		}else if(strcmp(argv[i], "--strategies") == 0){
			std::vector<std::string> strats = ReachabilitySearchStrategy::listStrategies();
			for(size_t i = 0; i < strats.size(); i++)
				printf("%s\n",strats[i].c_str());
		}else if(strcmp(argv[i], "--literal-query") == 0){
			queryString = argv[++i];
		}else if(strcmp(argv[i],"--strategy") == 0){
			strategy = argv[++i];
		}else if(strcmp(argv[i], "--list-queries") == 0){
			listformulas = true;
		} else if(strcmp(argv[i],"--gen-query") == 0){
			genRandState = true;
		} else if(strcmp(argv[i],"--help") == 0){
			printf("Usage: pete [net] [--query <query>] [--query-sumo <query>] [--help] [--strategies] [--strategy <strategy>]\n");
		} else
			filename = argv[i];
	}

	PetriNet* net = NULL;
	MarkVal* m0 = NULL;
	VarVal* v0 = NULL;
	std::vector<PNMLParser::Query> queries;
	{
		//Load the model
		ifstream modelfile(filename, ifstream::in);
		if(!modelfile){
			fprintf(stderr, "Argument Error: Model file \"%s\" couldn't be opened\n", filename.c_str());
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
		queries = parser.getQueries();

		// Close the file
		modelfile.close();
	}

	// List queries in file
	if(listformulas){
		for(std::vector<PNMLParser::Query>::iterator it = queries.begin(); it != queries.end(); it++)
			printf("%s\n",(*it).name.c_str());
		return 0;
	}

	// If generate random state
	if(genRandState){
		//RandomDFS dfs;
		//dfs.reachable(*net, m0, v0, NULL);
		RandomQueryGenerator rng;
		cout<<rng.gen(*net, m0, v0)<<"\n";
		return 0;
	}


	// Find the query
	if(queryString.empty()){
		for(std::vector<PNMLParser::Query>::iterator it = queries.begin(); it != queries.end(); it++){
			if((*it).name == queryname){
				queryString = (*it).text;
				break;
			}
		}
	}

	// Parse query and analyze
	PQL::Condition* query = PQL::ParseQuery(queryString);
	AnalysisContext context(*net);
	query->analyze(context);

	//Load up reachability engine
	ReachabilitySearchStrategy* strat;
	strat = ReachabilitySearchStrategy::createStrategy(strategy);
	ReachabilityResult result;

	clock_t startClock = clock();
	result = strat->reachable(*net, m0, v0, query);
	float finishTime = ((float)(clock() - startClock)) / (float)CLOCKS_PER_SEC;

	delete query;
	query = NULL;
	delete strat;
	strat = NULL;

	// Trim file name incase of alternate folder
	string name;
	bool chop = false;
	int index = 0;
	for(int i = 0; i < filename.size(); i++){
		if(filename.at(i) == '/'){
			chop = true;
			index = i;
		}
	}
	if(chop)
		name = filename.substr(index+1,filename.length()-1);

	delete query;
	query = NULL;
	delete strat;
	strat = NULL;
	//Print result
	std::string r;
	if(result.result() == ReachabilityResult::Satisfied)
		r = "Satisfied";
	else if(result.result() == ReachabilityResult::NotSatisfied)
		r = "Not satisfiable";
	else{
		r = "Unknown";
	}
	std::cout<<name
			 <<",\t"<<queryname
			 <<",\t"<<strategy
			 <<",\t"<<r
			 <<",\t"<<finishTime
			 <<",\t"<<result.expandedStates()
			 <<",\t"<<result.exploredStates()
			 <<",\t"<<result.pathLength()
			 <<std::endl;
	return 0;
}



























