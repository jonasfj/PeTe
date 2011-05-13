/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
	QPainterPath primaryShape();
private:
	QPainterPath textPath, tokenPath;
	void nameChanged();
	/** Number of tokens in the place */
	int _tokens;
};

#endif // PLACEITEM_H
