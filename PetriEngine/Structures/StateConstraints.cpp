#include "StateConstraints.h"

#include <lpsolve/lp_lib.h>
#include <assert.h>
#include <stdio.h>

/** Don't give lp_solve more than 30s to handle the problem */
#define OVER_APPROX_TIMEOUT				30
/** Number of M-traps to account for each place */
#define OVER_APPROX_TRAP_FACTOR			2

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
	for(int p = 0; p < nPlaces; p++)
		printf("%s: [%i, %i]\n", net.placeNames()[p].c_str(), pcs[p].min, pcs[p].max);
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
		set_col_name(lp, i+1, const_cast<char*>(net.transitionNames()[i].c_str()));

	// Start adding rows
	set_add_rowmode(lp, TRUE);

	REAL row[net.numberOfTransitions() + 1];
	for(size_t p = 0; p < nPlaces; p++){
		// Set row zero
		memset(row, 0, sizeof(REAL) * net.numberOfTransitions() + 1);
		for(size_t t = 0; t < net.numberOfTransitions(); t++){
			int d = net.outArc(t, p) - net.inArc(p, t);
			row[1+t] = d;
		}

		if(pcs[p].min == pcs[p].max &&
		   pcs[p].max != CONSTRAINT_INFTY){
			double target = pcs[p].min - m0[p];
			add_constraint(lp, row, EQ,  target);
		}else{
			// There's always a min, even zero is interesting
			double target = pcs[p].min - m0[p];
			add_constraint(lp, row, GE,  target);
			if(pcs[p].max != CONSTRAINT_INFTY){
				double target = pcs[p].max - m0[p];
				add_constraint(lp, row, LE,  target);
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
	/*printf("--------------\n");
	write_LP(lp, stdout);
	printf("--------------\n");*/

	//Set timeout, and handle a timeout
	set_timeout(lp, OVER_APPROX_TIMEOUT);

	// Attempt to solve the problem
	int result = solve(lp);

	// Limit on traps to test
	size_t traplimit = nPlaces * OVER_APPROX_TRAP_FACTOR;
	// Try to add a minimal trap constraint
	while((result == OPTIMAL || result == SUBOPTIMAL) && traplimit-- < 0){
		memset(row, 0, sizeof(REAL) * net.numberOfTransitions() + 1);
		// Get the firing vector
		get_variables(lp, row);
		// Compute the resulting marking
		MarkVal rMark[net.numberOfPlaces()];
		for(size_t p = 0; p < nPlaces; p++){
			rMark[p] = m0[p];
			for(size_t t = 0; t < net.numberOfTransitions(); t++)
				rMark[p] += (net.outArc(t, p) - net.inArc(p, t)) * (int)row[t];
		}

		// Find an M-trap
		BitField trap(minimalTrap(net, m0, rMark));

		/*printf("Initial marking:\n");
		for(size_t p = 0; p < nPlaces; p++)
			printf("\t %s = %i\n", net.placeNames()[p].c_str(), m0[p]);
		// Debug information
		printf("Firing vector:\n");
		for(size_t t = 0; t < net.numberOfTransitions(); t++)
			printf("\t %s = %i\n", net.transitionNames()[t].c_str(), (int)row[t]);
		printf("Result marking:\n");
		for(size_t p = 0; p < nPlaces; p++)
			printf("\t %s = %i\n", net.placeNames()[p].c_str(), rMark[p]);
		printf("Got trap: ");
		for(size_t p = 0; p < nPlaces; p++){
			if(trap.test(p))
				printf("%s, ", net.placeNames()[p].c_str());
		}
		printf("\n");*/

		//Break if there's no trap
		if(trap.none()) break;

		// Compute the new equation
		for(size_t t = 0; t < net.numberOfTransitions(); t++){
			row[1+t] = 0;
			for(size_t p = 0; p < nPlaces; p++)
				if(trap.test(p))
					row[1+t] += net.outArc(t, p) - net.inArc(p, t);
		}

		// Add a new row with target as greater than equal to 1
		set_add_rowmode(lp, TRUE);
		add_constraint(lp, row, GE,  1);
		set_add_rowmode(lp, FALSE);

		// Attempt to solve the again
		result = solve(lp);
	}

	// Delete the linear problem
	delete_lp(lp);
	lp = NULL;

	// Return true, if it was infeasible
	return result == INFEASIBLE;
}

BitField StateConstraints::maxTrap(const PetriNet& net,
								   BitField places,
								   const MarkVal* resultMarking) const{
	BitField next(places.size());
	BitField prev(places);
	do{
		prev = places;
		for(size_t i = 0; i < places.size(); i++)
			next.set(i, isInMaxTrap(net, i, places, resultMarking));
		places = next;
	}while(prev != next);
	return places;
}

bool StateConstraints::isInMaxTrap(const PetriNet& net,
								   size_t place,
								   const BitField& places,
								   const MarkVal* resultMarking) const{
	if(!places.test(place))
		return false;
	/*
		0 if M(p_i) = 1
		0 if there is (p_i , t) ∈ F such that x_j = 0
			for every p_j ∈ t•
		1 otherwise
	*/
	if(resultMarking[place] > 0)
		return false;

	for(unsigned int t = 0; t < net.numberOfTransitions(); t++){
		if(net.inArc(place, t) > 0){
			bool exclude = true;
			for(unsigned int j = 0; j < net.numberOfPlaces(); j++){
				if(net.outArc(t, j) > 0){
					exclude &= !places.test(j);
				}
			}
			if(exclude)
				return false;
		}
	}
	return true;
}

BitField StateConstraints::minimalTrap(const PetriNet& net,
									   const MarkVal* marking,
									   const MarkVal* resultMarking) const{
	const size_t nPlaces = net.numberOfPlaces();
	BitField trap(nPlaces);
	trap.set();
	trap = maxTrap(net, trap, resultMarking);
	if(!isMarked(trap, marking))
		return BitField(nPlaces).clear();

	//Get the exclusion candidates
	BitField EC(trap);
	BitField tmp(nPlaces);
	while(EC.any()){
		int exclude = EC.first();
		tmp = trap;
		tmp.clear(exclude);
		EC.clear(exclude);
		tmp = maxTrap(net, tmp, resultMarking);
		if(isMarked(tmp, marking)){
			trap = tmp;
			EC = tmp;
		}
	}
	return trap;
}

bool StateConstraints::isMarked(const BitField& places, const MarkVal* marking) const{
	for(size_t p = 0; p < places.size(); p++)
		if(places.test(p) && marking[p] > 0)
			return true;
	return false;
}

} // Structures
} // PetriEngines
