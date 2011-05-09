#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "../PetriEngine/PQL/PQL.h"
#include "../PetriEngine/PQL/PQLParser.h"


using namespace std;
using namespace PetriEngine::PQL;

map<string, SUMoQuery> ParseSUMoQueries(std::string filename){
	fstream filestr(filename, fstream::in);
	map<string, SUMoQuery> retMap;
	
	string queryString;
	while(!filestr.eof()){
		getline(filestr, queryString);

		//Do query stuff
		SUMoQuery query = ParseSUMoQuery(queryString);
		if(!query.query){
			fprintf(stderr, "Failed to parse query in %s", filename.c_str());
		}
		retMap[query.name] = query;
		queryString = "";

	}

	return retMap;
}
