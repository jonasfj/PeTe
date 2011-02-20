#ifndef ARCITEM_H
#define ARCITEM_H

#include <QGraphicsItem>
#include "netitem.h"

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
	NetItem* start() {return _startItem;}
	NetItem* end() {return _endItem;}
	void setEndPoint(QPointF end);
	void setEnd(NetItem* item);
private:
	NetItem* _startItem;
	NetItem* _endItem;
	QPointF _end;
};

#endif // ARCITEM_H

