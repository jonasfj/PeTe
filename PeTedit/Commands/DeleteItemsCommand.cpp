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
#include "DeleteItemsCommand.h"
#include "../NetItems/PetriNetScene.h"
#include "../NetItems/ArcItem.h"
#include "../NetItems/NetItem.h"

#include <QtGlobal>
#include <QList>

DeleteItemsCommand::DeleteItemsCommand(PetriNetScene* scene, QList<NetItem*> items, QList<ArcItem*> arcs, QUndoCommand* parent)
	: QUndoCommand(parent){
	_scene = scene;
	_items = items;
	_arcs = arcs;
	foreach(NetItem* item, _items){
		foreach(ArcItem* arc, item->ConnectedItems()) {
			if(!_arcs.contains(arc))
				_arcs.append(arc);
		}
	}
	if(_items.length() == 1)
		setText(QObject::tr("delete item \"%1\"").arg(items.first()->name()));
	else
		setText(QObject::tr("delete items"));
	added = true;
}

void DeleteItemsCommand::undo(){
	foreach(NetItem* item, _items)
		_scene->addItem(item);
	foreach(ArcItem* arc, _arcs){
		arc->registerAtEndPoints();
		_scene->addArc(arc);
	}
	added = true;
}

void DeleteItemsCommand::redo(){
	foreach(ArcItem* arc, _arcs){
		arc->unregisterAtEndPoints();
		_scene->removeArc(arc);
	}
	foreach(NetItem* item, _items)
		_scene->removeItem(item);
	added = false;
}

DeleteItemsCommand::~DeleteItemsCommand(){
	if(!added){
		foreach(NetItem* item, _items)
			delete item;
		_items.clear();
		foreach(ArcItem* arc, _arcs)
			delete arc;
		_arcs.clear();
	}
}
