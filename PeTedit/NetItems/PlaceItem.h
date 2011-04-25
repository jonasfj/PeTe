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

	int tokens() const {return _tokens;}
	void setTokens(int tokens);

	QPointF nearestPoint(QPointF to) const;
	int type() const;
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void updateTextPath();
	void updateTokenPath();
	QPainterPath opaqueArea() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);
private:
	QPainterPath textPath, tokenPath;
	void nameChanged();
	/** Number of tokens in the place */
	int _tokens;
};

#endif // PLACEITEM_H
