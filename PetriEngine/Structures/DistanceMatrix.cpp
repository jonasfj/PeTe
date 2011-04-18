#include "DistanceMatrix.h"

namespace PetriEngine{
namespace Structures{

DistanceMatrix::DistanceMatrix(const PetriNet& net){
	_dim = net.numberOfPlaces();
	_matrix = (unsigned int*)malloc(((_dim * _dim - _dim) / 2) * sizeof(unsigned int) + (_dim * _dim) * sizeof(unsigned int));
	pm = _matrix + ((_dim * _dim - _dim) / 2);
	memset(_matrix, INFINITE_DISTANCE, (_dim * _dim - _dim) / 2);
	generate(net);
}

void DistanceMatrix::generate(const PetriNet& net){
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
		for(size_t i = 0; i < _dim; i++){
			unsigned int min = INFINITE_DISTANCE;
			unsigned int pmin = 0;
			for(size_t p2 = 0; p2 < _dim; p2++){
				bool taken = false;
				for(size_t j = 0; j < i; j++)
					taken |= pm[p1 * _dim + i] == p2;
				if(taken && d(p1, p2) <= min){
					min = d(p1, p2);
					pmin = p2;
				}
			}
			pm[p1 * _dim + i] = pmin;
		}
	}
}



} // Structures
} // PetriEngine
