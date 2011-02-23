#ifndef PLACEEXPR_H
#define PLACEEXPR_H

#include "CTLExpr.h"

namespace PetriEngine { namespace CTL {
class PlaceExpr: public CTLExpr {
public:
	PlaceExpr(int placenr){
		this->_placenr = placenr;
	}
	int evaluate(const Mark* marking);
private:
	int _placenr;
};
}}

#endif // PLACEEXPR_H
