#include "DeleteItemCommand.h"
#include "../NetItems/PetriNetScene.h"
#include "../NetItems/ArcItem.h"
#include "../NetItems/NetItem.h"

#include <QtGlobal>
#include <QList>

DeleteItemCommand::DeleteItemCommand(PetriNetScene* scene, NetItem* item, QUndoCommand* parent)
	: QUndoCommand(parent){
	_item = item;
	_scene = scene;;
	ArcItem* arc;
	foreach(arc, _item->ConnectedItems()) {
		if(!_arcs.contains(arc))
			_arcs.append(arc);
	}
	setText(QObject::tr("Removed item \"%1\"").arg(item->name()));
}

void DeleteItemCommand::undo(){
	_scene->addItem(_item);
	ArcItem* arc;
	foreach(arc, _arcs){
		arc->registerAtEndPoints();
		_scene->addArc(arc);
	}
}

void DeleteItemCommand::redo(){
	ArcItem* arc;
	foreach(arc, _arcs){
		arc->unregisterAtEndPoints();
		_scene->removeArc(arc);
	}
	_scene->removeItem(_item);
}
