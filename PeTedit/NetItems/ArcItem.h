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
