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
#include "MoveItemsCommand.h"
#include "Commands.h"
#include "../NetItems/NetItem.h"
#include "../NetItems/PetriNetScene.h"


MoveItemsCommand::MoveItemsCommand(PetriNetScene* scene, QList<NetItem*> items, qreal dx, qreal dy){
	this->scene = scene;
	_items = items;
	_dx = dx;
	_dy = dy;
	this->setText("Moved " + QString::number(items.count()) + " items");
}

void MoveItemsCommand::undo(){
	foreach(NetItem* item, _items)
		item->moveBy(-_dx, -_dy);
	foreach(NetItem* item, _items)
		item->updateConnectedItems();
	scene->updateSceneRect();
}

void MoveItemsCommand::redo(){
	foreach(NetItem* item, _items)
		item->moveBy(_dx, _dy);
	foreach(NetItem* item, _items)
		item->updateConnectedItems();
	scene->updateSceneRect();
}

int MoveItemsCommand::id() const{
	return Commands::MoveItemsCommand;
}

bool MoveItemsCommand::mergeWith(const QUndoCommand *other){
	if(other->id() != id())
		return false;

	const MoveItemsCommand* move = static_cast<const MoveItemsCommand*>(other);

	//Check if we're moving the same items
	if(this->_items.count() != move->_items.count())
		return false;
	foreach(NetItem* item, move->_items){
		if(!this->_items.contains(item))
			return false;
	}

	//Merge the commands
	this->_dx += move->_dx;
	this->_dy += move->_dy;

	return true;
}
