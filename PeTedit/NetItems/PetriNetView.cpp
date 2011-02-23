#include "PetriNetView.h"

#include <QDebug>

PetriNetView::PetriNetView(QWidget *parent) : QGraphicsView(parent) {
}


void PetriNetView::wheelEvent(QWheelEvent* event){
	if(event->modifiers() & Qt::ControlModifier){
		QGraphicsView::ViewportAnchor anchor = transformationAnchor();
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		if(event->delta() > 0)
			scale(1.1, 1.1);
		else
			scale(1.0/1.1, 1.0/1.1);
		setTransformationAnchor(anchor);
		event->accept();
	}else
		QGraphicsView::wheelEvent(event);
}
