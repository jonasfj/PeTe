#ifndef ABSTRACTDTAPNBUILDER_H
#define ABSTRACTDTAPNBUILDER_H

namespace PetriEngine {

/** Abstract base class for all DTAPN builders */
class AbstractDTAPNBuilder{
public:
	virtual void addPlace(const std::string& name, int tokens) = 0;
	virtual void addTransition(const std::string& name) = 0;
	virtual void addInArc(const std::string& place, const std::string& transition, int startInterval, int endInterval) = 0;
	virtual void addOutArc(const std::string& transition, const std::string& place) = 0;
	virtual ~AbstractDTAPNBuilder(){}
};

} // PetriEngine

#endif // ABSTRACTDTAPNBUILDER_H
