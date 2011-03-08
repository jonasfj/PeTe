#include "EditPlaceCommand.h"
#include "../NetItems/PlaceItem.h"
#include <QtGlobal>

EditPlaceCommand::EditPlaceCommand(PlaceItem *place, const QString &name, int tokens):
	RenameItemCommand(place, name) {
		_tokens = tokens;
		this->setText(QObject::tr("Modified \"%1\" to \"%2\"").arg(place->name()).arg(name));
}

void EditPlaceCommand::swap() {
	PlaceItem* t = (PlaceItem*)_item;
	/* Token swap */
	int ts = t->tokens();
	t->setTokens(_tokens);
	_tokens = ts;
}

void EditPlaceCommand::undo(){
	swap();
	RenameItemCommand::undo();
}

void EditPlaceCommand::redo(){
	swap();
	RenameItemCommand::redo();
}
