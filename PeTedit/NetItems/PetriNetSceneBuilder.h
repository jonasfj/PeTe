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
#ifndef PETRINETSCENEBUILDER_H
#define PETRINETSCENEBUILDER_H

#include <PetriEngine/AbstractPetriNetBuilder.h>
#include "../Misc/AbstractQueryListBuilder.h"
#include "../Misc/QueryModel.h"

#include <QUndoGroup>

class PetriNetScene;
class PetriNetView;

/** Builder for PetriNetScene */
class PetriNetSceneBuilder : public PetriEngine::AbstractPetriNetBuilder, public AbstractQueryListBuilder
{
	struct ArcEntry{
		QString src;
		QString dst;
		int weight;
	};
public:
	/** undogroup for the scene, and parent for the scene */
	PetriNetSceneBuilder(QUndoGroup* undoGroup, PetriNetView* sceneParent);
	void addPlace(const std::string &name, int tokens, double x, double y);
	void addVariable(const std::string& name, int initialValue, int range);
	void addTransition(const std::string &name,
					   const std::string &conditions,
					   const std::string &assignments,
					   double x, double y);
	void addInputArc(const std::string &place, const std::string &transition, int weight);
	void addOutputArc(const std::string &transition, const std::string &place, int weight);
	/** Extension method for loading queries */
	void addQuery(const QueryModel::Query& query);
	/** Get the scene created, note you may only call this once */
	PetriNetScene* makeScene();
private:
	PetriNetScene* scene;
	QList<ArcEntry> arcs;
};

#endif // PETRINETSCENEBUILDER_H
