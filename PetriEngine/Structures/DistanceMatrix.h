#ifndef DISTANCEMATRIX_H
#define DISTANCEMATRIX_H

#include "../PetriNet.h"

#include <string.h>
#include <set>

namespace PetriEngine{
namespace Structures{

#define INFINITE_DISTANCE		(1<<20)

class DistanceMatrix {
public:
	DistanceMatrix(const PetriNet& net){
		_dim = net.numberOfPlaces();
		_matrix = new unsigned int[(_dim * _dim - _dim) / 2];
		pm = new unsigned int[_dim * _dim];
		memset(_matrix, INFINITE_DISTANCE, (_dim * _dim - _dim) / 2);
		generate(net);
	}
	unsigned int distance(unsigned int p1, unsigned int p2) const{
		if(p1 < p2)
			return _matrix[p1 * _dim + p2];
		return _matrix[p2 * _dim + p1];
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
	void generate(const PetriNet& net){
		// Generate the initial connections
		for(size_t p1 = 0; p1 < _dim; p1++){
			for(size_t t = 0; t < net.numberOfTransitions(); t++){
				if(net.inArc(p1,t)){
					for(size_t p2 = 0; p2 < _dim; p2++){
						if(net.outArc(t, p2)){
							d(p1, p2) = 1;
						}
					}
				}
			}
		}
		// Floyd-Warshall
		for(size_t k = 0; k < _dim; k++){
			for(size_t i = 0; i < _dim; i++){
				for(size_t j = 0; j < _dim; j++){
					unsigned int D = d(i,k) + d(k,j);
					if(d(i,k) == INFINITE_DISTANCE || d(k,j) == INFINITE_DISTANCE)
						D = INFINITE_DISTANCE;
					d(i, j) = d(i, j) < D ? d(i, j) : D;
				}
			}
		}
		// Generate place order lists
		for(size_t p1 = 0; p1 < _dim; p1++){
			std::set<unsigned int> taken;
			for(size_t i = 0; i < _dim; i++){
				unsigned int min = INFINITE_DISTANCE;
				unsigned int pmin = 0;
				for(size_t p2 = 0; p2 < _dim; p2++){
					if(!taken.count(p2) && d(p1, p2) <= min){
						min = d(p1, p2);
						pmin = p2;
					}
				}
				taken.insert(pmin);
				pm[p1 * _dim + i] = pmin;
			}
		}
	}
	/** Gets the distance between two places */
	unsigned int& d(unsigned int p1, unsigned int p2){
		if(p1 < p2)
			return _matrix[p1 * _dim + p2];
		return _matrix[p2 * _dim + p1];
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
	size_t _dim;
};

} // Structures
} // PetriEngine

#endif // DISTANCEMATRIX_H
