#ifndef DISTANCEMATRIX_H
#define DISTANCEMATRIX_H

#include "../PetriNet.h"

#include <string.h>
#include <set>
#include <stdlib.h>

namespace PetriEngine{
namespace Structures{

#define INFINITE_DISTANCE		(1<<20)

class DistanceMatrix {
public:
	DistanceMatrix(const PetriNet& net);
	~DistanceMatrix();
	unsigned int distance(unsigned int p1, unsigned int p2) const{
		//if(p1 < p2)
			return _matrix[p1 * _dim + p2];
		//return _matrix[p2 * _dim + p1];
	}
	double tokenCost(unsigned int p, int tokens, const MarkVal* m) const{
		double cost = 0;
		// Iterate distance vector for p
		for(size_t i = 0; i < _dim; i++){
			unsigned int place = pm[p * _dim + i];
			if(m[place]){
				if(distance(p, place) == INFINITE_DISTANCE)
					return INFINITE_DISTANCE;
				cost += (m[place] < tokens ? m[place] : tokens)  * distance(p, place);
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
		//if(p1 < p2)
			return _matrix[p1 * _dim + p2];
		//return _matrix[p2 * _dim + p1];
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
