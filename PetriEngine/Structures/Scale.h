#ifndef SCALE_H
#define SCALE_H

#include <math.h>

namespace PetriEngine{
namespace Structures{

/** Provides functionality to perform adaptive scaling of memory, etc */
class Scale{
public:
	Scale(double a = 0, double b = 0, double c = 0){
		this->a = a;
		this->b = b;
		this->c = c;
	}

	/** the input value is the value */
	int operator() (double x){
		double fx = a*(x*x) + b*x + c;
		return (int)floor(fx);
	}
private:
	double a;
	double b;
	double c;
};

} //Structures
} //PetriEngine

#endif // SCALE_H
