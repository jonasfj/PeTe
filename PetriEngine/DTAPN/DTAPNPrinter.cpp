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
#include "DTAPNPrinter.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace PetriEngine{
namespace DTAPN{

/** Auxiliary function for converting from int to string */
std::string i2s(int i){
	stringstream ss;
	ss<<i;
	return ss.str();
}


void DTAPNPrinter::addInputArc(const std::string &place, const std::string &transition, int startInterval, int endInterval){
	cerr << "("+place+") -["+ i2s(startInterval) + ","+ i2s(endInterval) +"]-> |"+transition+"|\n";
}

void DTAPNPrinter::addOutputArc(const std::string &transition, const std::string &place){
	cerr << "|"+transition + "| --> ("+ place + ")\n";
}

void DTAPNPrinter::addPlace(const std::string &name, int tokens, int maxInvariantAge, double, double){
	cerr << "("+ name + ","+i2s(tokens)+", <= " + i2s(maxInvariantAge) + ")\n";
}

void DTAPNPrinter::addTransition(const std::string &name, double, double){
	cerr << "|"+ name +"|\n";
}

} //DTAPN
} //PetriEngine
