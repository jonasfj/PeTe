#ifndef ARCITEM_H
#define ARCITEM_H

#include <QGraphicsItem>
#include "NetItem.h"

class ArcItem : public QGraphicsItem
{
public:
	ArcItem(NetItem* start, NetItem* end = NULL);
	int type() const;
	QRectF boundingRect() const;
	QPainterPath shape() const;
	QPainterPath opaqueArea() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);
	void updateEndPoints();
	bool isInputArc() const {return _startItem->type() == NetEntity::PlaceItem;}
	NetItem* start() {return _startItem;}
	NetItem* end() {return _endItem;}
	void setEndPoint(QPointF end);
	void setEnd(NetItem* item);
	void registerAtEndPoints();
	void unregisterAtEndPoints();
	int weight() const {return _weight;}
	void setWeight(int weight) {
		this->prepareGeometryChange();
		_weight = weight;
		this->update();
		if(isInputArc())
			if(end())
				end()->update();
		updateTextPath();
	}
private:
	QPointF _cachedPoint;
	void updateArrowPath();
	QPainterPath _cachedArrowPath;
	void updateTextPath();
	QPainterPath _cachedTextPath;
	NetItem* _startItem;
	NetItem* _endItem;
	QPointF _end;
	int _weight;
};

#endif // ARCITEM_H
