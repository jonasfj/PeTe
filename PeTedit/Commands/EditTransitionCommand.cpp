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
