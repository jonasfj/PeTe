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
#include "InsertArcCommand.h"

#include "../NetItems/ArcItem.h"
#include "../NetItems/PetriNetScene.h"

InsertArcCommand::InsertArcCommand(PetriNetScene* scene, ArcItem* arc){
	_scene = scene;
	_arc = arc;
	_arc->unregisterAtEndPoints();
	_removed = true;
	this->setText("Insert arc");
}

InsertArcCommand::~InsertArcCommand(){
	if(_removed && _arc)
		delete _arc;
	_arc = NULL;
}

void InsertArcCommand::undo(){
	_arc->unregisterAtEndPoints();
	_scene->removeArc(_arc);
	_removed = true;
}

void InsertArcCommand::redo(){
	_scene->addArc(_arc);
	_arc->registerAtEndPoints();
	_removed = false;
	_arc->updateEndPoints();
}

