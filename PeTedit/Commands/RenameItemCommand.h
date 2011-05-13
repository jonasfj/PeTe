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
#ifndef RENAMEITEMCOMMAND_H
#define RENAMEITEMCOMMAND_H

#include <QUndoCommand>
#include <QtGlobal>

class NetItem;

class RenameItemCommand : public QUndoCommand {
public:
	RenameItemCommand(NetItem* item, const QString& name);
	void undo();
	void redo();
protected:
	QString _newname;
	NetItem* _item;
private:
	void swap();
};

#endif // RENAMEITEMCOMMAND_H
