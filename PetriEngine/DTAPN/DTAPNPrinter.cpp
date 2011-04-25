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

void DTAPNPrinter::addPlace(const std::string &name, int tokens, double, double){
	cerr << "("+ name + ","+i2s(tokens)+")\n";
}

void DTAPNPrinter::addTransition(const std::string &name, double, double){
	cerr << "|"+ name +"|\n";
}

} //DTAPN
} //PetriEngine
