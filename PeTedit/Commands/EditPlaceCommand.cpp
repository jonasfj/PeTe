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
