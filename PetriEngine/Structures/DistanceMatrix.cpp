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
#include "DistanceMatrix.h"

namespace PetriEngine{
namespace Structures{

DistanceMatrix::DistanceMatrix(const PetriNet& net){
	_dim = net.numberOfPlaces();
	_matrix = new unsigned int[(_dim * _dim - _dim) + (_dim * _dim)];
	pm = _matrix + (_dim * _dim - _dim);
	generate(net);
}

DistanceMatrix::~DistanceMatrix(){
	if(_matrix)
		delete[] _matrix;
	_matrix = NULL;
}

#define MIN(a,b)	(a < b ? a : b)
#define MAX(a,b)	(a > b ? a : b)

void DistanceMatrix::generate(const PetriNet& net){
	for(size_t i = 0; i < _dim; i++){
		for(size_t j = 0; j < _dim; j++){
			d(j, i) = INFINITE_DISTANCE;
		}
	}
	// Generate the initial connections
	for(size_t p1 = 0; p1 < _dim; p1++){
		d(p1, p1) = 0;
		for(size_t t = 0; t < net.numberOfTransitions(); t++){
			if(net.inArc(p1,t) - net.outArc(t, p1) > 0){
				for(size_t p2 = 0; p2 < _dim; p2++){
					if(net.outArc(t, p2) - net.inArc(p2, t) > 0){
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
				if(d(i,k) >= INFINITE_DISTANCE || d(k,j) >= INFINITE_DISTANCE)
					D = INFINITE_DISTANCE;
				d(i, j) = MIN(d(i, j), D);
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
					taken |= (pm[p1 * _dim + j] == p2);
				if(!taken && d(p2, p1) <= min){
					min = d(p2, p1);
					pmin = p2;
				}
			}
			pm[p1 * _dim + i] = pmin;
		}
	}
}



} // Structures
} // PetriEngine
