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
#ifndef DISTANCEMATRIX_H
#define DISTANCEMATRIX_H

#include "../PetriNet.h"

#include <string.h>
#include <set>
#include <stdlib.h>
#include <stdio.h>


namespace PetriEngine{
namespace Structures{

#define INFINITE_DISTANCE		(1<<20)

class DistanceMatrix {
public:
	DistanceMatrix(const PetriNet& net);
	~DistanceMatrix();
	unsigned int distance(unsigned int p1, unsigned int p2) const{
		return _matrix[p1 * _dim + p2];
	}
	double tokenCost(unsigned int p, int tokens, const MarkVal* m) const{
		double cost = 0;
		// Iterate distance vector for p
		for(size_t i = 0; i < _dim; i++){
			unsigned int place = pm[p * _dim + i];
			if(m[place]){
				if(distance(place, p) == INFINITE_DISTANCE)
					return INFINITE_DISTANCE;
				cost += (m[place] < tokens ? m[place] : tokens)  * distance(place, p);
				tokens -= m[place];
				if(tokens <= 0)
					return cost;
			}
		}
		return INFINITE_DISTANCE;
	}
private:
	/** Instantiates the initial matrix */
	void generate(const PetriNet& net);
	/** Gets the distance between two places */
	unsigned int& d(unsigned int p1, unsigned int p2){
		return _matrix[p1 * _dim + p2];
	}
	/** Underlying distance matrix */
	unsigned int* _matrix;
	/** Place matrix ordered by increasing relative distance
	 * p1 : p1 ...
	 * p2 : p2 ...
	 * p3 : p3 ...
	 * Where ... is the places closest to the first one... e.g. ordered lists
	 * of places...
	 */
	unsigned int* pm;
	unsigned int _dim;
};

} // Structures
} // PetriEngine

#endif // DISTANCEMATRIX_H
