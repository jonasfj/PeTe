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
#include "InsertPlaceCommand.h"

#include "../NetItems/PlaceItem.h"
#include "../NetItems/PetriNetScene.h"

#include <QtGlobal>

InsertPlaceCommand::InsertPlaceCommand(PetriNetScene *scene, QPointF position){
	_scene = scene;
	_removed = true;
	int number = 0;
	QString name;
	do{
		number += 1;
		name =  "P" + QString::number(number);
	}while(_scene->findNetItem(name));
	_place = new PlaceItem(position, name);
	this->setText("Created \"" + name + "\"");
}

InsertPlaceCommand::~InsertPlaceCommand(){
	if(_removed)
		delete _place;
	_place = NULL;
}

void InsertPlaceCommand::undo(){
	Q_ASSERT(_removed == false);
	_scene->removeNetItem(_place);
	_removed = true;
}

void InsertPlaceCommand::redo(){
	Q_ASSERT(_removed == true);
	_scene->addNetItem(_place);
	_removed = false;
}
