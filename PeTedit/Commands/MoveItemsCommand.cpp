#include "MoveItemsCommand.h"
#include "Commands.h"

MoveItemsCommand::MoveItemsCommand(QList<QGraphicsItem*> items, qreal dx, qreal dy){
	_items = items;
	_dx = dx;
	_dy = dy;
	this->setText("Moved " + QString::number(items.count()) + " items");
}

void MoveItemsCommand::undo(){
	foreach(QGraphicsItem* item, _items)
		item->moveBy(-_dx, -_dy);
}

void MoveItemsCommand::redo(){
	foreach(QGraphicsItem* item, _items)
		item->moveBy(_dx, _dy);
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
	foreach(QGraphicsItem* item, move->_items){
		if(!this->_items.contains(item))
			return false;
	}

	//Merge the commands
	this->_dx += move->_dx;
	this->_dy += move->_dy;

	return true;
}
