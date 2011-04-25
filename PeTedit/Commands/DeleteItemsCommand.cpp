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
