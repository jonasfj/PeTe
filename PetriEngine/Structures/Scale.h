#ifndef SCALE_H
#define SCALE_H

namespace PetriEngine{
namespace Structures{

/** Provides functionality to perform adaptive scaling of memory, etc */
class Scale
{
public:

	/** The descent function */
	enum ScalingFunction{
		Linear	= 0,
		Hyperbola = 1
	};

	Scale(int xstart, int ymax, int scale, ScalingFunction function)
	{
		this->x_start=xstart;
		this->y_max=ymax;
		this->scale=scale;
		this->function=function;
	}
	int calculate(int x){
		// Start from inital offset
		if(x < x_start)
			x = x_start;

		float fx;

		if(function == Linear)
			fx = ymax-x;
		 else if(function == Hyperbola)
			fx = (1/x);

		return (int)(scale*fx);
	}
private:
	int x_start;
	int y_max;
	int scale;
	ScalingFunction function;
};

} //Structures
} //PetriEngine

#endif // SCALE_H
