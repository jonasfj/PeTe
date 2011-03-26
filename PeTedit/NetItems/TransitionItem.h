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

	/** Checks if the transition is enabled */
	bool enabled() const;

	// The pre-conditions for the place.
	QString preConditions() const{
		return _preConditions;
	}
	void setPreConditions(const QString conditions);

	// The post-conditions for the place
	QString postConditions() const{
		return _postConditions;
	}
	void setPostConditions(const QString conditions);

	void refreshTooltip();

	QRectF boundingRect() const;
	QPainterPath shape() const;
	QPainterPath opaqueArea() const;
	QPainterPath textPath() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);

private:
	/** condition queries associated with it **/
	QString _preConditions;
	QString _postConditions;

};

#endif // TRANSITIONITEM_H
