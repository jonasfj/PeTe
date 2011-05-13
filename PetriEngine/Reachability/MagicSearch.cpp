#include "MagicSearch.h"
#include "../Structures/EnhancedPriorityQueue.h"

#define	MEMORY_BOUND		1024*1024*512

using namespace PetriEngine::Structures;
using namespace PetriEngine::PQL;

namespace PetriEngine{
namespace Reachability{

ReachabilityResult MagicSearch::reachable(const PetriNet &net,
										  const MarkVal *m0,
										  const VarVal *v0,
										  PQL::Condition *query){
	SmartStateAllocator<MEMORY_BOUND> allocator(net);
	SmartStateSet states(net);
	EnhancedPriorityQueue<Step> queue;

	_dm = new Structures::DistanceMatrix(net);
	{	//Compute constraints
		ConstraintAnalysisContext context(net);
		query->findConstraints(context);
		if(context.canAnalyze)
			contraints = context.retval;
	}

	//Create s0
	SmartState* s0 = allocator.createStoredState();
	memcpy(s0->marking(), m0, sizeof(MarkVal) * net.numberOfPlaces());
	memcpy(s0->valuation(), v0, sizeof(VarVal) * net.numberOfVariables());
	states.add(s0, s0->marking(), s0->valuation());
	Step step0;
	step0.depth = 0;
	step0.lastApprox = INT_MAX;
	step0.lastStored = 0;
	step0.state = s0;
	queue.push(0, step0);

	//Temporary marking and valuation to work with
	MarkVal tmpM[net.numberOfPlaces()];
	VarVal tmpV[net.numberOfVariables()];
	SmartState* ns = allocator.createStoredState();
	SmartState* ls = allocator.createState();

	//Statistics
	int lastReport = 0;
	BigInt expanded = 0, explored = 0;
	size_t max = 1;

	//Main loop
	while(!queue.empty()){
		// Report progress if needed
		if(lastReport++ & 1<<17){
			lastReport = 0;
			if(queue.size() > max)
				max = queue.size();
			this->reportProgress((double)(max - queue.size()) / ((double)(max)));
			if(this->abortRequested())
				return ReachabilityResult(ReachabilityResult::Unknown,
										  "Query aborted!");
		}

		//Pop stuff of the queue
		Step step = queue.pop(depthFirst);
		expanded++; //Cound expanded states

		// Get current state
		const MarkVal* m;
		const VarVal* v;
		if(step.state->stored()){
			m = step.state->marking();
			v = step.state->valuation();
		}else{
			step.state->getState(net, tmpM, tmpV);
			m = tmpM;
			v = tmpV;
		}

		//Attempt to exclude by over-approx
		if(step.lastApprox >= approxScale(step.depth)){
			if(canExcludeByOverApprox(net, m, v))
				continue;
			step.lastApprox = 0;
		}

		for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
			//Fire the transition
			if(net.fire(t, m, v, ns->marking(), ns->valuation())){
				//Determine whether or not to store the entire state
				bool storeCurrentState = step.lastStored >= storeScale(allocator.percentMemoryUsed());// storeScale(allocator.percentMemoryUsed());
				SmartState* storeState;
				if(storeCurrentState)
					storeState = ns;
				else
					storeState = ls;
				storeState->setParent(step.state);
				storeState->setTransition(t);

				//Add it to the state set
				if(states.add(storeState, ns->marking(), ns->valuation())){
					explored++; //Count explored states

					//Test the query
					if(query->evaluate(EvaluationContext(ns->marking(), ns->valuation()))){
						printf("\nmemory usage: %f\n",allocator.percentMemoryUsed());
						return ReachabilityResult(ReachabilityResult::Satisfied,
												  "Query was satified!",
												  expanded,
												  explored,
												  0 /*TODO: Support path depth */);
					}

					//Make the next step
					Step nextstep;
					nextstep.depth = step.depth + 1;
					nextstep.lastApprox = step.lastApprox + 1;
					if(storeState == ns)
						nextstep.lastStored = 0;
					else
						nextstep.lastStored = step.lastStored + 1;
					nextstep.state = storeState;

					//Push step on the queue
					double p = priority(ns->marking(), ns->valuation(), query, net);
					queue.push(p, nextstep);

					//Allocate new memory, depending on what was stored
					if(storeState == ns)
						ns = allocator.createStoredState();
					else
						ls = allocator.createState();
				}
			}
		}
	}
	return ReachabilityResult(ReachabilityResult::NotSatisfied,
							  "Query cannot be satisfied!",
							  expanded,
							  explored);
}

double MagicSearch::priority(const MarkVal *marking,
							 const VarVal *valuation,
							 const PQL::Condition *query,
							 const PetriNet &net){
	PQL::DistanceContext context(net,
								 _distanceStrategy,
								 marking,
								 valuation,
								 _dm);
	return query->distance(context);
}

bool MagicSearch::canExcludeByOverApprox(const PetriNet& net, const MarkVal* m, const VarVal* v){
	bool isImpossible = contraints.size() > 0;
	for(size_t i = 0; i < contraints.size(); i++){
		isImpossible &= contraints[i]->isImpossible(net, m, v);
		if(!isImpossible) break;
	}
	return isImpossible;
}

} // Reachability
} // PetriEngine
