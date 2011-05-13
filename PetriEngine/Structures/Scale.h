/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
