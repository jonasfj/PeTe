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
