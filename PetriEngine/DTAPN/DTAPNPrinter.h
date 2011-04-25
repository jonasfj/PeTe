#ifndef DTAPNPRINTER_H
#define DTAPNPRINTER_H

#include "AbstractDTAPNBuilder.h"

namespace PetriEngine{
namespace DTAPN{


class DTAPNPrinter : public AbstractDTAPNBuilder
{
public:

	void addPlace(const std::string& name, int tokens, double, double);

	void addTransition(const std::string& name, double, double);

	void addInputArc(const std::string& place, const std::string& transition, int startInterval, int endInterval);

	void addOutputArc(const std::string& transition, const std::string& place);
};

} //DTAPN
} //PetriEngine

#endif // DTAPNPRINTER_H
