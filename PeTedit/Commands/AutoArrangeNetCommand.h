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
#ifndef AUTOARRANGENETCOMMAND_H
#define AUTOARRANGENETCOMMAND_H

#include <QUndoCommand>
#include "../NetItems/PetriNetScene.h"
#include <PetriEngine/AbstractPetriNetBuilder.h>

/** Auto arrange command */
class AutoArrangeNetCommand : public QUndoCommand
{
public:
	/** Automatically arrange items on scene */
	AutoArrangeNetCommand(PetriNetScene* scene);
	/** Automatically align items orientation */
	AutoArrangeNetCommand(PetriNetScene* scene, QList<NetItem*> items, Qt::Orientation alignOn);
	void redo();
	void undo();

	/** Auxiliary class of building a list of NetItems and their positions */
	class NetItemPositionListBuilder : public PetriEngine::AbstractPetriNetBuilder{
	public:
		NetItemPositionListBuilder(PetriNetScene* scene) {
			this->scene = scene;
		}
		/** Representation of item and position */
		struct NetItemPosition{
			NetItem* item;
			double x, y;
		};
		void addVariable(const std::string &name, int initialValue, int range);
		void addPlace(const std::string &name, int tokens, double x, double y);
		void addTransition(const std::string &name, const std::string &conditions, const std::string &assignments, double x, double y);
		void addInputArc(const std::string &place, const std::string &transition, int weight);
		void addOutputArc(const std::string &transition, const std::string &place, int weight);
		/** Output the final NetItemPositionList */
		const QList<NetItemPosition>& makeNetItemPositionList() const { return list; }
	private:
		PetriNetScene* scene;
		QList<NetItemPosition> list;
	};
private:
	PetriNetScene* _scene;
	/** Positions before auto arrange */
	QList<NetItemPositionListBuilder::NetItemPosition> beforeList;
	/** Positions after auto arrange */
	QList<NetItemPositionListBuilder::NetItemPosition> afterList;
};

#endif // AUTOARRANGENETCOMMAND_H
