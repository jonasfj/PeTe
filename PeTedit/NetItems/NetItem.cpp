#include "NetItem.h"
#include "ArcItem.h"

QVariant NetItem::itemChange(GraphicsItemChange change, const QVariant &value){
	if(change == QGraphicsItem::ItemPositionHasChanged){
		foreach(ArcItem* item, this->_connectedItems)
			item->updateEndPoints();
	}
	return QGraphicsItem::itemChange(change, value);
}
