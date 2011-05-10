#ifndef SCALE_H
#define SCALE_H

namespace PetriEngine{
namespace Structures{

/** Provides functionality to perform adaptive scaling of memory, etc */
class Scale
{
public:

	Scale(double a, double b, double c)
	{
		this->a = a;
		this->b = b;
		this->c = c;
	}

	/** the input value is the value */
	int operator()(double x){
		// Start from inital offset
		if(x < x_start)
			x = x_start;
		double fx = a*(x*x) + b*x + c;
		return (int)fx;
	}
private:
	double a;
	double b;
	double c;
};

} //Structures
} //PetriEngine

#endif // SCALE_H
