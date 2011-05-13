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
#include "PetriNetSceneBuilder.h"

#include "PetriNetScene.h"
#include "PlaceItem.h"
#include "ArcItem.h"
#include "TransitionItem.h"
#include "../Misc/VariableModel.h"

PetriNetSceneBuilder::PetriNetSceneBuilder(QUndoGroup* undoGroup, PetriNetView* sceneParent){
	scene = new PetriNetScene(undoGroup, sceneParent);
}

void PetriNetSceneBuilder::addPlace(const std::string &name, int tokens, double x, double y){
	PlaceItem* item = new PlaceItem(QPointF(x,y), name.c_str());
	item->setTokens(tokens);
	scene->addNetItem(item);
}

void PetriNetSceneBuilder::addVariable(const std::string& name, int initialValue, int range){
	scene->variables()->addVariable(name.c_str(), initialValue, range);
}

void PetriNetSceneBuilder::addTransition(const std::string &name,
										 const std::string &conditions,
										 const std::string &assignments,
										 double x,
										 double y){
	TransitionItem* item = new TransitionItem(QPointF(x,y), name.c_str());
	//TODO: Perhaps use a new constructor for this?
	item->setPreConditions(conditions.c_str());
	item->setPostConditions(assignments.c_str());
	scene->addNetItem(item);
}

void PetriNetSceneBuilder::addInputArc(const std::string &place, const std::string &transition, int weight){
	arcs.append({place.c_str(), transition.c_str(), weight});
}

void PetriNetSceneBuilder::addOutputArc(const std::string &transition, const std::string &place, int weight){
	arcs.append({transition.c_str(), place.c_str(), weight});
}


/** Extension method for loading queries */
void PetriNetSceneBuilder::addQuery(const QueryModel::Query& query){
	scene->queries()->insertQuery(query);
}

PetriNetScene* PetriNetSceneBuilder::makeScene(){
	foreach(ArcEntry entry, arcs){
		NetItem* start = scene->findNetItem(entry.src);
		NetItem* end = scene->findNetItem(entry.dst);
		if(start && end && start->type() != end->type()){
			ArcItem* item = new ArcItem(start, end);
			item->setWeight(entry.weight);
			scene->addArc(item);
		}
	}
	arcs.clear();
	return scene;
}
