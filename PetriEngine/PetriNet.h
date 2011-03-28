#ifndef PETRINET_H
#define PETRINET_H

#include <string>
#include <vector>
#include <climits>

namespace PetriEngine{

namespace PQL{
	class Condition;
	class AssignmentExpression;
}

class PetriNetFactory;

/** Type used for holding markings values */
typedef int MarkVal;
/** Type used for holding variable values */
typedef int VarVal;

typedef int Mark;

#define MARK_INF					INT_MAX

/** Marking representation as an integer array.
  * Use DECLARE_MARKING macro to initialise the marking on the heap,
  * and ALLOCATE_MARKING to initialise on the stack
  */
typedef Mark* Marking;
#define DECLARE_MARKING(m, s)		int m[s]
#define ALLOCATE_MARKING(m, s)		Marking m = (Marking)new int[s]
#define NEW_MARKING(s)				(Marking)new int[s]
#define GET_TOKENS(m, o)			m[o]
#define SET_TOKENS(m, o, v)			m[o] = v

/** Assignment representation as an integer array.
  * Use DECLARE_ASSIGNMENT macro to initialise the assignment on the heap,
  * and ALLOCATE_ASSIGNMENT to initialise on the stack
  */
typedef Mark* Assignment;
#define DECLARE_ASSIGNMENT(a, s)	int a[s]
#define ALLOCATE_ASSIGNMENT(a, s)	Assignment a = (Assignment)new int[s]
#define GET_VALUE(a, o)				a[o]
#define SET_VALUE(a, o, v)			a[o] = v

/** Efficient representation of PetriNet */
class PetriNet
{
	struct AnnotationPair{
		PQL::Condition* condition;
		PQL::AssignmentExpression* assignment;
	};
	PetriNet(int places, int transitions, int variables);
public:
	/** Fire transition if possible and store result in result */
	bool fire(unsigned int transition,
			  const MarkVal* marking,
			  const VarVal* assignment,
			  MarkVal* resultMarking,
			  VarVal* resultAssignment) const;

	/** Returns the place offset, or -1 if not found */
	int lookupPlace(const std::string& name) const;
	/** Returns the variable offset, or -1 if not found */
	int lookupVariable(const std::string& name) const;
private:
	std::string* _places;
	std::string* _transitions;
	std::string* _variables;
	size_t _nPlaces, _nTransitions, _nVariables;
	MarkVal* _transitionMatrix;
	VarVal* _ranges;
	AnnotationPair* _annotations;
	friend class PetriNetFactory;
};

} // PetriEngine

#endif // PETRINET_H
