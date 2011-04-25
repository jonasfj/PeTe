#ifndef ABSTRACTDTAPNBUILDER_H
#define ABSTRACTDTAPNBUILDER_H

#include <string>

namespace PetriEngine {
namespace DTAPN {

/** Abstract base class for all DTAPN builders */
class AbstractDTAPNBuilder{
public:
	virtual void addPlace(const std::string& name,
						  int tokens,
						  double x =0,
						  double y = 0) = 0;

	virtual void addTransition(const std::string& name,
							   double x =0,
							   double y = 0) = 0;

	virtual void addInputArc(const std::string& place,
							 const std::string& transition,
							 int startInterval,
							 int endInterval) = 0;

	virtual void addOutputArc(const std::string& transition,
							  const std::string& place) = 0;

	virtual ~AbstractDTAPNBuilder(){}
};

} // DTAPN
} // PetriEngine

#endif // ABSTRACTDTAPNBUILDER_H
