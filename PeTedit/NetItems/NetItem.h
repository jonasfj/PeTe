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
#ifndef NETITEM_H
#define NETITEM_H

#include <QGraphicsItem>
#include <QPointF>
#include <QtGlobal>

class ArcItem;

namespace NetEntity{
	/** Different node types, register new types here */
	enum ItemType {
		UndefinedItem = QGraphicsItem::UserType,
		PlaceItem,
		TransitionItem,
		ArcItem
	};
}

/** Abstract class for all items, not arcs */
class NetItem : public QGraphicsItem{
public:
	/** Returns the point on the edges of the node closest to to.*/
	virtual QPointF nearestPoint(QPointF to) const = 0;

	const QString& name() const {return _name;}
	void setName(const QString& name) {
		Q_ASSERT(!name.contains(" "));
		this->prepareGeometryChange();
		_name = name;
		nameChanged();
	}

	/** Register a connected item
	 *
	 * Registered items will be updated when this object is moved.
	 * They will also be handled by the undo command that removes
	 * this object.
	 */
	void registerConnectedItem(ArcItem* item){
		_connectedItems.append(item);
		this->update();
	}

	/** Unregister a connected item */
	void unregisterConnectedItem(ArcItem* item){
		_connectedItems.removeAll(item);
		this->update();
	}

	const QList<ArcItem*> ConnectedItems() const{
		return _connectedItems;
	}

	/** True, if the item is a netitem */
	static bool isNetItem(QGraphicsItem* item){
		return item->type() == NetEntity::PlaceItem ||
			   item->type() == NetEntity::TransitionItem;
	}

	/** Primary shape in scene coordinates */
	virtual QPainterPath primaryShape() = 0;

	/** Update positions of connected items */
	void updateConnectedItems();

protected:
private:
	/** Notify of name change */
	virtual void nameChanged() = 0;
	/** List of connected items (arcs etc.) */
	QList<ArcItem*> _connectedItems;
	QString _name;
};


#endif // NETITEM_H
