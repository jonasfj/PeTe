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
#ifndef INSERTTRANSITIONCOMMAND_H
#define INSERTTRANSITIONCOMMAND_H

#include <QUndoCommand>
#include <QPointF>

class PetriNetScene;
class TransitionItem;

/** Command to insert a transition, for undo/redo support */
class InsertTransitionCommand : public QUndoCommand
{
public:
	InsertTransitionCommand(PetriNetScene* scene, QPointF position);
	~InsertTransitionCommand();
	TransitionItem* transition() {return _transition;}
	void undo();
	void redo();
private:
	PetriNetScene* _scene;
	TransitionItem* _transition;
	bool _removed;
};


#endif // INSERTTRANSITIONCOMMAND_H
