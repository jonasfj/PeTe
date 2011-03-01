#ifndef NETITEM_H
#define NETITEM_H

#include <QGraphicsItem>
#include <QPointF>

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
	NetItem(){
		this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	}

	/** Returns the point on the edges of the node closest to to.*/
	virtual QPointF nearestPoint(QPointF to) const = 0;

	const QString& name() const {return _name;}
	void setName(const QString& name) {
		this->prepareGeometryChange();
		_name = name;
	}

	/** Register a connected item
	 *
	 * Registered items will be updated when this object is moved.
	 * They will also be handled by the undo command that removes
	 * this object.
	 */
	void registerConnectedItem(ArcItem* item){
		_connectedItems.append(item);
	}

	/** Unregister a connected item */
	void unregisterConnectedItem(ArcItem* item){
		_connectedItems.removeAll(item);
	}

	const QList<ArcItem*> ConnectedItems() const{
		return _connectedItems;
	}

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
	/** List of connected items (arcs etc.) */
	QList<ArcItem*> _connectedItems;
	QString _name;
};


#endif // NETITEM_H
