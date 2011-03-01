#ifndef PLACEITEM_H
#define PLACEITEM_H

#include "NetItem.h"

#include <QGraphicsItem>
#include <QPainterPath>
#include <QPainter>

/** Place in a Petri Net */
class PlaceItem : public NetItem
{
public:
	PlaceItem(QPointF position, QString name);

	// Getters/setters

	QString name() {return _name;}
	void setName(QString name) {_name = name;}

	int tokens() const {return _tokens;}
	void setTokens(int tokens){_tokens = tokens;}

	QPointF nearestPoint(QPointF to) const;
	int type() const;
	QRectF boundingRect() const;
	QPainterPath shape() const;
	QPainterPath opaqueArea() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);
private:
	QString _name;
	/** Number of tokens in the place */
	int _tokens;
};

#endif // PLACEITEM_H
