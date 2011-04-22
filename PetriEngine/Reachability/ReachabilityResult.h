#ifndef REACHABILITYRESULT_H
#define REACHABILITYRESULT_H

namespace PetriEngine { namespace Reachability {

/** Result of a reachability search */
class ReachabilityResult{
public:
	/** Types of results */
	enum Result{
		/** The query was satisfied */
		Satisfied,
		/** The query cannot be satisfied */
		NotSatisfied,
		/** We're unable to say if the query can be satisfied */
		Unknown
	};

	/** Create a new instance of ReachabilityResult */
	ReachabilityResult(Result result = Unknown, const std::string& explanation = "", int expandedStates = -1, int exploredStates = -1){
		_result = result;
		_explanation = explanation;
		_expandedStates = expandedStates;
		_exploredStates = exploredStates;
	}

	/** Gets a human readable explanation */
	const std::string& explanation() const { return _explanation;}
	/** Gets the formal result */
	Result result() const{ return _result; }
	/** Gets the number of expanded states */
	int expandedStates(){ return _expandedStates; }
	int exploredStates(){ return _exploredStates; }
private:
	std::string _explanation;
	Result _result;
	int _expandedStates;
	int _exploredStates;
};

} // Reachability
} // PetriEngine

#endif // REACHABILITYRESULT_H
