#include "PetriNetView.h"

#include <QDebug>
#include <QTransform>

PetriNetView::PetriNetView(QWidget *parent) : QGraphicsView(parent) {}


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
