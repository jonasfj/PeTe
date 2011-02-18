#ifndef PLACEITEM_H
#define PLACEITEM_H

#include "netitem.h"

#include <QGraphicsItem>
#include <QPainterPath>
#include <QPainter>

/** Place in a Petri Net */
class PlaceItem : public NetItem
{
public:
	PlaceItem(QPointF position, QString name);

	QString name() {return _name;}
	void setName(QString name) {_name = name;}

	QPointF nearestPoint(QPointF to) const;
	int type() const;
	QRectF boundingRect() const;
	QPainterPath shape() const;
	QPainterPath opaqueArea() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);
private:
	QString _name;
};

#endif // PLACEITEM_H
