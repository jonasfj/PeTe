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
#include "PetriNetView.h"

#include <QDebug>
#include <QTransform>

PetriNetView::PetriNetView(QWidget *parent) : QGraphicsView(parent) {
	this->setRenderHints(QPainter::Antialiasing |
						 QPainter::SmoothPixmapTransform |
						 QPainter::TextAntialiasing);
	this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}


void PetriNetView::wheelEvent(QWheelEvent* event){
	if(event->modifiers() & Qt::ControlModifier){
		QGraphicsView::ViewportAnchor anchor = transformationAnchor();
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		if(event->delta() > 0)
			scaleBy(1.1);
		else
			scaleBy(1.0/1.1);
		setTransformationAnchor(anchor);
		event->accept();
	}else
		QGraphicsView::wheelEvent(event);
}

void PetriNetView::scaleBy(qreal factor){
	//If we're outside the range, abort
	if(((currentScale() == minScale) && (factor < 1.0)) ||
	   ((currentScale() == maxScale) && (factor > 1.0)))
		return;

	if (currentScale() * factor < minScale && factor < 1.0)
		factor = minScale / currentScale();
	else if(currentScale() * factor > maxScale && factor > 1.0)
		factor = maxScale / currentScale();

	scale(factor, factor);

	// Changed scale factor
	emit zoomChanged(currentScale() * 100);
}

void PetriNetView::setZoom(double percent){
	qreal targetScale = (qreal)percent / 100.0;
	qreal factor = targetScale / currentScale();
	scaleBy(factor);
}
