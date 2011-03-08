#include "EditArcCommand.h"
#include "../NetItems/ArcItem.h"
#include <QtGlobal>

EditArcCommand::EditArcCommand(ArcItem *arc, int weight) {
	this->_arc = arc;
	this->_newWeight = weight;
	this->setText(QObject::tr("Modified arc from \"%1\" to \"%2\"").arg(arc->start()->name()).arg(arc->end()->name()));
}

void EditArcCommand::swap() {
	int t = _arc->weight();
	_arc->setWeight(_newWeight);
	_newWeight = t;
}

void EditArcCommand::undo() {
	swap();
}

void EditArcCommand::redo() {
	swap();
}
