#ifndef PETRINET_H
#define PETRINET_H

#include <string>
#include <vector>
#include <climits>

namespace PetriEngine{

class PetriNetFactory;

/** Marking representation as an integer array.
  * Use DECLARE_MARKING macro to initialise the marking on the heap,
  * and ALLOCATE_MARKING to initialise on the stack
  */
typedef int* Marking;
#define DECLARE_MARKING(m, s)		int m[s]
#define ALLOCATE_MARKING(m, s)		Marking m = (Marking)new int[s]
#define NEW_MARKING(s)				(Marking)new int[s]
#define GET_TOKENS(m, o)			m[o]
#define SET_TOKENS(m, o, v)			m[o] = v

/** Assignment representation as an integer array.
  * Use DECLARE_ASSIGNMENT macro to initialise the assignment on the heap,
  * and ALLOCATE_ASSIGNMENT to initialise on the stack
  */
typedef int* Assignment;
#define DECLARE_ASSIGNMENT(a, s)	int a[s]
#define ALLOCATE_ASSIGNMENT(a, s)	int Assignment a = (Assignment)new int[s]
#define GET_VALUE(a, o)				a[o]
#define SET_VALUE(a, o, v)			a[o] = v

#define MARK_INF					INT_MAX

/** Efficient representation of PetriNet*/
class PetriNet
{
public:
	PetriNet(int places, int transitions);
	/** Fire transition if possible and store result in result */
	bool fire(int transition, const Marking marking, Marking result) const;
	int nPlaces();
	int nTransitions();
	/** Creates an empty marking initialized to 0 */
	Marking makeEmptyMarking();
	/** Returns the place offset in the places */
	int lookupPlace(const std::string& name) const;
	int lookupVariable(const std::string& name) const;
private:
	std::string* _placeNames;
	std::string* _transitionNames;
	Marking _transitions;
	int _nPlaces;
	int _nTransitions;
	friend class PetriNetFactory;
};

} // PetriEngine

#endif // PETRINET_H
