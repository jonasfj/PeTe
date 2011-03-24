#include "EditTransitionCommand.h"
#include "../NetItems/TransitionItem.h"
#include <QtGlobal>


EditTransitionCommand::EditTransitionCommand(TransitionItem *item, const QString &name, const QString &preconditions, const QString &postconditions)
	:RenameItemCommand(item,name){
		_newPreConditions=preconditions;
		_newPostConditions=postconditions;
		this->setText(QObject::tr("Modified \"%1\" to \"%2\"").arg(item->name()).arg(item->name()));
}

void EditTransitionCommand::swap() {
	TransitionItem* t = (TransitionItem*)_item;
	/* swap conditions*/
	QString prec = t->preConditions();
	QString posc = t->postConditions();

	t->setPreConditions(_newPreConditions);
	t->setPostConditions(_newPostConditions);

	_newPreConditions=prec;
	_newPostConditions=posc;
}

void EditTransitionCommand::undo(){
	swap();
	RenameItemCommand::undo();
}

void EditTransitionCommand::redo(){
	swap();
	RenameItemCommand::redo();
}
