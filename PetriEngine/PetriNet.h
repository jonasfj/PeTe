#ifndef PETRINET_H
#define PETRINET_H

#include <string>
#include <vector>

namespace PetriEngine{

class PetriNetFactory;

/** Definition of mark, a marking is an array of markings */
typedef int Mark;

/** Efficient representation of PetriNet*/
class PetriNet
{
public:
	PetriNet(int places, int transitions);
	/** Fire transition if possible and store result in result */
	bool fire(int transition, const Mark* marking, Mark* result) const;
private:
	std::string* _placeNames;
	std::string* _transitionNames;
	Mark* _transitions;
	int _nPlaces;
	int _nTransitions;
	friend class PetriNetFactory;
};

} // PetriEngine

#endif // PETRINET_H
