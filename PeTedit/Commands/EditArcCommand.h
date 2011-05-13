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
#ifndef EDITARCCOMMAND_H
#define EDITARCCOMMAND_H

#include <QUndoCommand>
#include <QtGlobal>

class ArcItem;

class EditArcCommand : public QUndoCommand {
public:
	EditArcCommand(ArcItem* arc, int weight);
	void undo();
	void redo();
private:
	void swap();
	ArcItem* _arc;
	int _newWeight;
};

#endif // EDITARCCOMMAND_H
