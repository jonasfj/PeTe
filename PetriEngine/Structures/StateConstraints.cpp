#include "StateConstraints.h"

#include <lpsolve/lp_lib.h>
#include <assert.h>
#include <stdio.h>

namespace PetriEngine{
namespace Structures{

/** Attempts to merge two StateConstraints, return NULL if can't be merged */
StateConstraints* StateConstraints::merge(const StateConstraints* c) const{
	StateConstraints* nc = new StateConstraints(nPlaces, nVars);
	for(size_t p = 0; p < nPlaces; p++){
		if(!nc->setPlaceMax(p, pcs[p].max) ||
		   !nc->setPlaceMax(p, c->pcs[p].max) ||
		   !nc->setPlaceMin(p, pcs[p].min) ||
		   !nc->setPlaceMin(p, c->pcs[p].min)){
			delete nc;
			return NULL;
		}
	}
	for(size_t v = 0; v < nVars; v++){
		if(!nc->setVarMax(v, vcs[v].max) ||
		   !nc->setVarMax(v, c->vcs[v].max) ||
		   !nc->setVarMin(v, vcs[v].min) ||
		   !nc->setVarMin(v, c->vcs[v].min)){
			delete nc;
			return NULL;
		}
	}
	return nc;
}

/** True, if every marking satisfied by subset is also satisfied by this */
bool StateConstraints::isSuperSet(const StateConstraints* subset) const{
	for(size_t p = 0; p < nPlaces; p++){
		if(pcs[p].max < subset->pcs[p].max)
			return false;
		if(pcs[p].min > subset->pcs[p].min)
			return false;
	}
	for(size_t v = 0; v < nVars; v++){
		if(vcs[v].max < subset->vcs[v].max)
			return false;
		if(vcs[v].min > subset->vcs[v].min)
			return false;
	}
	return true;
}

void StateConstraints::reset(){
	for(size_t p = 0; p < nPlaces; p++){
		pcs[p].max = CONSTRAINT_INFTY;
		pcs[p].min = 0;
	}
	for(size_t v = 0; v < nVars; v++){
		vcs[v].max = CONSTRAINT_INFTY;
		vcs[v].min = 0;
	}
}

/** Merge the two sets of StateConstraints such that one from A and one from B is always satisfied, when one in the return value is
 * @remarks This will take ownership of the provided StateConstraints, and delete them or own them
 */
std::vector<StateConstraints*> StateConstraints::mergeAnd(const std::vector<StateConstraints*>& A, const std::vector<StateConstraints*>& B){
	std::vector<StateConstraints*> retval;
	typedef std::vector<StateConstraints*>::const_iterator iter;
	for(iter a = A.begin(); a != A.end(); a++){
		for(iter b = B.begin(); b != B.end(); b++){
			StateConstraints* nc = (*a)->merge(*b);
			if(nc)
				retval.push_back(nc);
		}
		delete *a;
	}
	for(iter b = B.begin(); b != B.end(); b++)
		delete *b;
	return retval;
}

/** Merge the two sets of StateConstraints such that either on from A or B is always satisfied, when one in the return value is
 * @remarks This will take ownership of the provided StateConstraints, and delete them or own them
 */
std::vector<StateConstraints*> StateConstraints::mergeOr(const std::vector<StateConstraints*>& A, const std::vector<StateConstraints*>& B){
	std::vector<StateConstraints*> retval, excluded;
	typedef std::vector<StateConstraints*>::const_iterator iter;
	for(iter a = A.begin(); a != A.end(); a++){
		bool exclude = false;
		for(iter b = B.begin(); b != B.end(); b++){
			exclude |= (*b)->isSuperSet(*a) && !(*b)->isEqual(*a);
			if(exclude) break;
		}
		if(!exclude)
			retval.push_back(*a);
		else
			excluded.push_back(*a);
	}
	for(iter b = B.begin(); b != B.end(); b++){
		bool exclude = false;
		for(iter a = A.begin(); a != A.end(); a++){
			exclude |= (*a)->isSuperSet(*b);
			if(exclude) break;
		}
		if(!exclude)
			retval.push_back(*b);
		else
			excluded.push_back(*b);
	}
	for(iter e = excluded.begin(); e != excluded.end(); e++)
		delete *e;
	return retval;
}

StateConstraints* StateConstraints::requirePlaceMin(const PetriNet& net, int place, MarkVal value){
	StateConstraints* c = new StateConstraints(net);
	c->setPlaceMin(place, value);
	return c;
}

StateConstraints* StateConstraints::requirePlaceMax(const PetriNet& net, int place, MarkVal value){
	StateConstraints* c = new StateConstraints(net);
	c->setPlaceMax(place, value);
	return c;
}

StateConstraints* StateConstraints::requirePlaceEqual(const PetriNet& net, int place, MarkVal value){
	StateConstraints* c = new StateConstraints(net);
	c->setPlaceMax(place, value);
	c->setPlaceMin(place, value);
	return c;
}


StateConstraints* StateConstraints::requireVarMin(const PetriNet& net, int var, MarkVal value){
	StateConstraints* c = new StateConstraints(net);
	c->setVarMin(var, value);
	return c;
}

StateConstraints* StateConstraints::requireVarMax(const PetriNet& net, int var, MarkVal value){
	StateConstraints* c = new StateConstraints(net);
	c->setVarMax(var, value);
	return c;
}

StateConstraints* StateConstraints::requireVarEqual(const PetriNet& net, int var, MarkVal value){
	StateConstraints* c = new StateConstraints(net);
	c->setVarMax(var, value);
	c->setVarMin(var, value);
	return c;
}

/** Attempts to solve using lp_solve, returns True if the net cannot satisfy these constraints! */
bool StateConstraints::isImpossible(const PetriNet& net,
									const MarkVal* m0,
									const VarVal*) const{
	assert(nPlaces == net.numberOfPlaces());
	assert(nVars == net.numberOfVariables());

	/*printf("-------\n");
	for(int p = 0; p < nPlaces; p++){
		printf("%s: [%i, %i]\n", net.placeNames()[p].c_str(), pcs[p].min, pcs[p].max);
	}
	printf("-------\n");*/

	// Create linary problem
	lprec* lp;
	lp = make_lp(0, net.numberOfTransitions());	// One variable for each entry in the firing vector
	assert(lp);
	if(!lp) return false;

	// Set verbosity
	set_verbose(lp, IMPORTANT);

	// Set transition names (not strictly needed)
	for(size_t i = 0; i < net.numberOfTransitions(); i++)
		set_col_name(lp, i+1, (char*)net.transitionNames()[i].c_str());

	// Start adding rows
	set_add_rowmode(lp, TRUE);

	REAL row[net.numberOfTransitions() + 1];
	for(int p = 0; p < nPlaces; p++){
		// Set row zero
		memset(row, 0, sizeof(REAL) * net.numberOfTransitions() + 1);
		for(size_t t = 0; t < net.numberOfTransitions(); t++){
			int d = net.outArc(t, p) - net.inArc(p, t);
			row[1+t] = d;
		}

		if(pcs[p].min == pcs[p].max &&
		   pcs[p].max != CONSTRAINT_INFTY){
			double target = pcs[p].min - m0[p];
			bool ok = add_constraint(lp, row, EQ,  target);
			assert(ok);
		}else{
			// There's always a min, even zero is interesting
			double target = pcs[p].min - m0[p];
			bool ok = add_constraint(lp, row, GE,  target);
			assert(ok);
			if(pcs[p].max != CONSTRAINT_INFTY){
				double target = pcs[p].max - m0[p];
				bool ok = add_constraint(lp, row, LE,  target);
				assert(ok);
			}
		}
	}

	// Finished adding rows
	set_add_rowmode(lp, FALSE);

	// Create objective
	memset(row, 0, sizeof(REAL) * net.numberOfTransitions() + 1);
	for(size_t t = 0; t < net.numberOfTransitions(); t++)
		row[1+t] = 1;	// The sum the components in the firing vector

	// Set objective
	set_obj_fn(lp, row);

	// Minimize the objective
	set_minim(lp);

	// Set variables as integer variables
	for(size_t i = 0; i < net.numberOfTransitions(); i++)
		set_int(lp, 1+i, TRUE);

	// Write it to stdout
	//write_LP(lp, stdout);

	// Attempt to solve the problem
	int result = solve(lp);

	// Delete the linear problem
	delete_lp(lp);
	lp = NULL;

	// Return true, if it was infeasible
	return result == INFEASIBLE;
}


} // Structures
} // PetriEngines
