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
#include "InsertTransitionCommand.h"

#include "../NetItems/TransitionItem.h"
#include "../NetItems/PetriNetScene.h"

InsertTransitionCommand::InsertTransitionCommand(PetriNetScene* scene, QPointF position){
	_removed = true;
	_scene = scene;
	int number = 0;
	QString name;
	do{
		number += 1;
		name =  "T" + QString::number(number);
	}while(_scene->findNetItem(name));
	_transition = new TransitionItem(position, name);
	this->setText("Created \"" + name + "\"");
}

InsertTransitionCommand::~InsertTransitionCommand(){
	if(_removed)
		delete _transition;
	_transition = NULL;
}

void InsertTransitionCommand::undo(){
	Q_ASSERT(_removed == false);
	_scene->removeNetItem(_transition);
	_removed = true;
}

void InsertTransitionCommand::redo(){
	Q_ASSERT(_removed == true);
	_scene->addNetItem(_transition);
	_removed = false;
}
