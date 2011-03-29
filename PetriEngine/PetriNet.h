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

#define EMPTY_MARKING(size)			(MarkVal*)calloc(size, sizeof(MarkVal));
#define EMPTY_ASSIGNMENT(size)		(VarVal*)calloc(size, sizeof(VarVal));

#define MARK_INF					INT_MAX

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
	/** Get a transition vector */
	const MarkVal* transitionVector(unsigned int transition) const {
		return _transitionMatrix + _nPlaces * transition;
	}
	unsigned int numberOfTransitions() const {return _nTransitions;}
	unsigned int numberOfVariables() const {return _nVariables;}
	unsigned int numberOfPlaces() const {return _nPlaces;}
	/** Returns the place offset, or -1 if not found */
	int lookupPlace(const std::string& name) const;
	/** Returns the variable offset, or -1 if not found */
	int lookupVariable(const std::string& name) const;
private:
	std::string* _places;
	std::string* _transitions;
	std::string* _variables;
	/** Number of x variables */
	size_t _nPlaces, _nTransitions, _nVariables;
	MarkVal* _transitionMatrix;
	VarVal* _ranges;
	AnnotationPair* _annotations;
	friend class PetriNetFactory;
};

} // PetriEngine

#endif // PETRINET_H
