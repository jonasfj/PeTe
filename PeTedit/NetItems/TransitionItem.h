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

	/** The pre-conditions for the place. */
	const QString& preConditions() const{
		return _preConditions;
	}
	void setPreConditions(const QString& conditions){
		_preConditions = conditions;
		refreshTooltip();
	}

	/** The post-conditions for the place */
	const QString& postConditions() const{
		return _postConditions;
	}
	void setPostConditions(const QString& conditions){
		_postConditions = conditions;
		refreshTooltip();
	}

	void refreshTooltip();

	QRectF boundingRect() const;
	QPainterPath shape() const;
	QPainterPath opaqueArea() const;
	void updateTextPath();
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);
	QPainterPath primaryShape();
private:
	void nameChanged();
	QPainterPath textPath;
	/** condition queries associated with it **/
	QString _preConditions;
	QString _postConditions;
};

#endif // TRANSITIONITEM_H
