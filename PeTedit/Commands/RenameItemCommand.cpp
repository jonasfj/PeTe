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
