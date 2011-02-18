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
	void updatePosition();
private:
	NetItem* _start;
	NetItem* _end;
};

#endif // ARCITEM_H

