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
#ifndef PETRINETBUILDER_H
#define PETRINETBUILDER_H

#include "AbstractPetriNetBuilder.h"
#include <vector>
#include <string>

#include "PetriNet.h"

namespace PetriEngine{


/** Builder for building engine representations of PetriNets */
class PetriNetBuilder : public AbstractPetriNetBuilder
{
	struct Arc{
		std::string place;
		std::string transition;
		int weight;
	};
public:
	PetriNetBuilder(bool JIT = false);
	void addVariable(const std::string& name, int initialValue, int range);
	void addPlace(const std::string& name, int tokens, double x, double y);
	void addTransition(const std::string& name,
					   const std::string& condition,
					   const std::string& assignment,
					   double x,
					   double y);
	void addInputArc(const std::string& place,
					 const std::string& transition,
					 int weight);
	void addOutputArc(const std::string& transition, const std::string& place, int weight);
	/** Make the resulting petri net, you take ownership */
	PetriNet* makePetriNet();
	/** Make the resulting initial marking, you take ownership */
	MarkVal* makeInitialMarking();
	/** Make the resulting initial assignment, you take ownership */
	VarVal* makeInitialAssignment();
private:
	std::vector<std::string> places;
	std::vector<std::string> transitions;
	std::vector<std::string> conditions;
	std::vector<std::string> assignments;
	std::vector<std::string> variables;
	std::vector<int> initialVariableValues;
	std::vector<int> ranges;
	std::vector<Arc> inputArcs;
	std::vector<Arc> outputArcs;
	std::vector<int> initialMarking;
	bool _jit;
};

}

#endif // PETRINETBUILDER_H

