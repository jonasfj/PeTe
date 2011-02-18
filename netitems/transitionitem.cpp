#include "transitionitem.h"
#include "netitem.h"

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

int TransitionItem::type() const{
	return NetEntity::TransitionItem;
}

QPointF TransitionItem::nearestPoint(QPointF to) const{
	QLineF line = QLineF(this->pos(), to);
	line = line.unitVector();
	qreal x = line.dx() * (RECT_W/2);
	qreal y = line.dy() * (RECT_W/2);
	return this->pos() + QPointF(x,y);
}

QRectF TransitionItem::boundingRect() const{
	QPainterPath path;
	path.addRect(-SELECTION_RECT_W/2, -SELECTION_RECT_H/2, SELECTION_RECT_W, SELECTION_RECT_H);
	return path.controlPointRect();
}

QPainterPath TransitionItem::shape() const{
	QPainterPath path;
	path.addRect(-RECT_W/2, -RECT_H/2, RECT_W, RECT_H);
	return path;
}

QPainterPath TransitionItem::opaqueArea() const{
	return shape();
}

void TransitionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*){
	painter->setBrush(Qt::SolidPattern);
	painter->drawPath(shape());
	if(this->isSelected()){
		painter->setBrush(Qt::NoBrush);
		QPen pen(Qt::DotLine);
		pen.setColor(QColor(0, 0, 0, 128));
		painter->setPen(pen);
		painter->drawRect(-SELECTION_RECT_W/2, -SELECTION_RECT_H/2, SELECTION_RECT_W, SELECTION_RECT_H);
	}
}
