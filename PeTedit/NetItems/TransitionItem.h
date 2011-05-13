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
