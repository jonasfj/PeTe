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
#ifndef MOVEITEMSCOMMAND_H
#define MOVEITEMSCOMMAND_H

#include <QUndoCommand>
#include <QPointF>
#include <QGraphicsItem>

class PetriNetScene;
class NetItem;

class MoveItemsCommand : public QUndoCommand
{
public:
	MoveItemsCommand(PetriNetScene* scene, QList<NetItem*> items, qreal dx, qreal dy);
	void undo();
	void redo();
	int id() const;
	bool mergeWith(const QUndoCommand *other);
private:
	PetriNetScene* scene;
	QList<NetItem*> _items;
	qreal _dx, _dy;
};

#endif // MOVEITEMSCOMMAND_H
