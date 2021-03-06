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
#include "TransitionItem.h"
#include "NetItem.h"
#include "ArcItem.h"
#include "PlaceItem.h"

#include <QPen>
#include <QBrush>
#include <QColor>

#define RECT_W	7.5
#define RECT_H	20
#define SELECTION_RECT_W	(7.5+9)
#define SELECTION_RECT_H	29

TransitionItem::TransitionItem(QPointF position, QString name) : NetItem(){
	this->setName(name);
	this->setPos(position);

	//Configure the underlying graphics item
	this->setFlag(QGraphicsItem::ItemIsMovable, true);
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void TransitionItem::refreshTooltip(){
	this->setToolTip(this->name()+ "\n - pre: "+_preConditions + "\n - post: " +_postConditions);
}

int TransitionItem::type() const{
	return NetEntity::TransitionItem;
}

bool TransitionItem::enabled() const{
	if(this->ConnectedItems().count() == 0)
		return false;

	foreach(ArcItem* arc, this->ConnectedItems()){
		if(arc->isInputArc()){
			PlaceItem* start = dynamic_cast<PlaceItem*>(arc->start());
			// Input should *always* be a place
			Q_ASSERT(start != NULL);
			if(start->tokens() < arc->weight())
				return false;
		}
	}
	return true;
}

void TransitionItem::updateTextPath() {
	QPainterPath path;
	// Draw text label for transition
	path.addText(0, 0, QFont("", 6), this->name());

	QRectF moved = path.boundingRect();
	qreal yoffset = path.boundingRect().height();
	moved.moveCenter(QPointF(0, yoffset*3));
	path.translate(moved.center() - path.boundingRect().center());

	textPath = path;
}

QPointF TransitionItem::nearestPoint(QPointF to) const{
	QLineF line = QLineF(this->pos(), to);
	if(line.length() == 0)	//Avoid bugs with NaN
		return to;
	line = line.unitVector();
	qreal x = line.dx() * RECT_W / 2;
	qreal y = line.dy() * RECT_H / 2;
	return this->pos() + QPointF(x,y);
}

QRectF TransitionItem::boundingRect() const{
	QPainterPath path;
	path.addRect(-SELECTION_RECT_W/2, -SELECTION_RECT_H/2, SELECTION_RECT_W, SELECTION_RECT_H);
	path.addPath(textPath);
	return path.controlPointRect();
}

QPainterPath TransitionItem::shape() const{
	QPainterPath path;
	path.addRect(-RECT_W/2, -RECT_H/2, RECT_W, RECT_H);
	path.addPath(textPath);
	return path;
}

QPainterPath TransitionItem::opaqueArea() const{
	return shape();
}

void TransitionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*){
	painter->setBrush(Qt::SolidPattern);

	QPainterPath path;
	path.addRect(-RECT_W/2, -RECT_H/2, RECT_W, RECT_H);
	painter->drawPath(path);

	painter->setPen(Qt::NoPen);
	painter->drawPath(textPath);

	if(this->isSelected()){
		painter->setBrush(Qt::NoBrush);
		QPen pen(Qt::DotLine);
		pen.setColor(QColor(0, 0, 0, 128));
		painter->setPen(pen);
		painter->drawRect(-SELECTION_RECT_W/2, -SELECTION_RECT_H/2, SELECTION_RECT_W, SELECTION_RECT_H);
	}
}


void TransitionItem::nameChanged(){
	updateTextPath();
}

QPainterPath TransitionItem::primaryShape(){
	QPainterPath path;
	path.addRect(-RECT_W/2, -RECT_H/2, RECT_W, RECT_H);
	path.translate(pos());
	return path;
}

