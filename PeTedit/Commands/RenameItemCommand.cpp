#include "RenameItemCommand.h"
#include "../NetItems/NetItem.h"
#include <QtGlobal>

RenameItemCommand::RenameItemCommand(NetItem *item, const QString &name) : _newname(name) {
	_item = item;
	this->setText(QObject::tr("Renamed \"%1\" to \"%2\"").arg(item->name()).arg(name));
}

void RenameItemCommand::swap() {
	QString t = _item->name();
	_item->setName(_newname);
	_newname = t;
}

void RenameItemCommand::undo() {
	swap();
}

void RenameItemCommand::redo() {
	swap();
}
