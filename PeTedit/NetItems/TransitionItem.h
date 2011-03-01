#ifndef TRANSITIONITEM_H
#define TRANSITIONITEM_H

#include "NetItem.h"

#include <QGraphicsItem>
#include <QPainterPath>
#include <QPainter>

/** Transition in a Petri Net */
class TransitionItem : public NetItem
{
public:
	TransitionItem(QPointF position, QString name);

	QPointF nearestPoint(QPointF to) const;
	int type() const;
	QRectF boundingRect() const;
	QPainterPath shape() const;
	QPainterPath opaqueArea() const;
	QPainterPath textPath() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);
};

#endif // TRANSITIONITEM_H
