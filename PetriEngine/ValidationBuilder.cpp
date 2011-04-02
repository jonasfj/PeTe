#include "ValidationBuilder.h"

#include "PQL/PQLParser.h"

#include <assert.h>
#include <sstream>
#include <set>

using namespace std;

namespace PetriEngine{


/** Auxiliary function for converting into to string */
inline string int2string(int i){
	stringstream ss;
	ss<<i;
	return ss.str();
}

ValidationBuilder::ValidationBuilder() : AbstractPetriNetBuilder() {}

void ValidationBuilder::addVariable(const std::string& name,
									int initialValue,
									int range){
	_varNames.push_back(name);
	if(initialValue > range){	//TODO: Figure out if this is > or >=
		string msg = "Initial value " + int2string(initialValue)
					 + " is larger than range " + int2string(range);
		_errors.push_back(ValidationError(name, msg));
	}
}

void ValidationBuilder::addPlace(const std::string& name,
								 int tokens,
								 double,
								 double){
	_placeNames.push_back(name);
	if(tokens < 0){
		string msg = "Initial marking " + int2string(tokens)
					 + " is less than zero";
		_errors.push_back(ValidationError(name, msg));
	}
}

void ValidationBuilder::addTransition(const std::string& name,
									  const std::string& conditions,
									  const std::string& assignments,
									  double,
									  double){
	_transitionNames.push_back(name);
	_conditions.push_back(conditions);
	_assignments.push_back(assignments);
}

void ValidationBuilder::addInputArc(const std::string& place,
									const std::string& transition,
									int weight){
	_inputArcs.push_back(Arc(place, transition));
	if(weight < 0){
		string msg = "Weight is " + int2string(weight)
					 + " is less than zero";
		_errors.push_back(ValidationError(place, transition, msg));
	}
}

void ValidationBuilder::addOutputArc(const std::string& transition,
									 const std::string& place,
									 int weight){
	_outputArcs.push_back(Arc(transition, place));
	if(weight < 0){
		string msg = "Weight is " + int2string(weight)
					 + " is less than zero";
		_errors.push_back(ValidationError(transition, place, msg));
	}
}

/** Get the number of matching identifiers */
int ValidationBuilder::countMatchingIds(const std::string& id){
	int count = 0;
	for(size_t i = 0; i < _varNames.size(); i++)
		if(_varNames[i] == id) count++;
	for(size_t i = 0; i < _placeNames.size(); i++)
		if(_placeNames[i] == id) count++;
	for(size_t i = 0; i < _transitionNames.size(); i++)
		if(_transitionNames[i] == id) count++;
	return count;
}

bool ValidationBuilder::validate(){
	//Check for duplicate identifiers
	set<string> reportedDuplicates;	//Use a set to avoid reporting them more than once
	//Check variable names
	for(size_t i = 0; i < _varNames.size(); i++){
		const string& id = _varNames[i];
		int count = countMatchingIds(id);
		assert(count >= 1);
		if(count > 1 && reportedDuplicates.count(id) == 0){
			reportedDuplicates.insert(id);
			_errors.push_back(ValidationError(id,
											  "The identifiers must be unique, \""
											  + id + "\" is shared by "
											  + int2string(count) + " entities"));
		}
	}
	//Check place names
	for(size_t i = 0; i < _placeNames.size(); i++){
		const string& id = _placeNames[i];
		int count = countMatchingIds(id);
		assert(count >= 1);
		if(count > 1 && reportedDuplicates.count(id) == 0){
			reportedDuplicates.insert(id);
			_errors.push_back(ValidationError(id,
											  "The identifiers must be unique, \""
											  + id + "\" is shared by "
											  + int2string(count) + " entities"));
		}
	}
	//Check transition names
	for(size_t i = 0; i < _transitionNames.size(); i++){
		const string& id = _transitionNames[i];
		int count = countMatchingIds(id);
		assert(count >= 1);
		if(count > 1 && reportedDuplicates.count(id) == 0){
			reportedDuplicates.insert(id);
			_errors.push_back(ValidationError(id,
											  "The identifiers must be unique, \""
											  + id + "\" is shared by "
											  + int2string(count) + " entities"));
		}
	}

	//Check all input arcs
	for(size_t i = 0; i < _inputArcs.size(); i++){
		const Arc& arc = _inputArcs[i];
		bool foundPlace = false;
		bool foundTransition = false;
		//Look for place
		for(size_t j = 0; j < _placeNames.size(); j++){
			foundPlace |= _placeNames[j] == arc.start;
			if(foundPlace) break;
		}
		//Look for transition
		for(size_t j = 0; j < _transitionNames.size(); j++){
			foundTransition |= _transitionNames[j] == arc.end;
			if(foundTransition) break;
		}
		//Report error
		if(!foundPlace || !foundTransition){
			string msg;
			if(!foundPlace && !foundTransition)
				msg = "Neigther end-points found";
			else if(!foundPlace)
				msg = "Start-place \"" + arc.start + "\" not found";
			else
				msg = "End-transition \"" + arc.end + "\" not found";
			_errors.push_back(ValidationError(arc.start,
											  arc.end,
											  msg));
		}
	}
	//Check all output arcs
	for(size_t i = 0; i < _outputArcs.size(); i++){
		const Arc& arc = _outputArcs[i];
		bool foundPlace = false;
		bool foundTransition = false;
		//Look for transition
		for(size_t j = 0; j < _transitionNames.size(); j++){
			foundTransition |= _transitionNames[j] == arc.start;
			if(foundTransition) break;
		}
		//Look for place
		for(size_t j = 0; j < _placeNames.size(); j++){
			foundPlace |= _placeNames[j] == arc.end;
			if(foundPlace) break;
		}
		//Report error
		if(!foundPlace || !foundTransition){
			string msg;
			if(!foundPlace && !foundTransition)
				msg = "Neigther end-points found";
			else if(!foundPlace)
				msg = "End-place \"" + arc.end + "\" not found";
			else
				msg = "Start-transition \"" + arc.start + "\" not found";
			_errors.push_back(ValidationError(arc.start,
											  arc.end,
											  msg));
		}
	}

	//Attempt to parse all non-empty conditions
	for(size_t i = 0; i < _conditions.size(); i++){
		if(_conditions[i].empty()) continue;
		PQL::Condition* cond = PQL::ParseQuery(_conditions[i]);
		if(cond){
			PQL::AnalysisContext context(_placeNames, _varNames);
			cond->analyze(context);
			for(size_t j = 0; j < context.errors().size(); j++){
				_errors.push_back(ValidationError(_transitionNames[i],
												  context.errors()[j],
												  true));
			}
			delete cond;
			cond = NULL;
		}else
			_errors.push_back(ValidationError(_transitionNames[i],
											  "Unable to parse non-empty condition"));
	}
	//Attempt to parse all non-empty assignments
	for(size_t i = 0; i < _assignments.size(); i++){
		if(_assignments[i].empty()) continue;
		PQL::AssignmentExpression* a = PQL::ParseAssignment(_assignments[i]);
		if(a){
			PQL::AnalysisContext context(_placeNames, _varNames);
			a->analyze(context);
			for(size_t j = 0; j < context.errors().size(); j++){
				_errors.push_back(ValidationError(_transitionNames[i],
												  context.errors()[j],
												  false));
			}
			delete a;
			a = NULL;
		}else
			_errors.push_back(ValidationError(_transitionNames[i],
											  "Unable to parse non-empty assignment"));
	}

	return _errors.empty();
}



} // PetriEngine
