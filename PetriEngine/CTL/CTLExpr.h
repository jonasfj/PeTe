#ifndef CTLEXPR_H
#define CTLEXPR_H

#include "../PetriNet.h"

namespace PetriEngine{ namespace CTL{
class CTLExpr {
public:
	virtual int evaluate(const Mark* marking) = 0;
};
}}

#endif // CTLEXPR_H
