#include "placeitem.h"

#include <QPen>
#include <QBrush>
#include <QColor>

#include "netitem.h"

#define CIRCLE_SIZE			7.5
#define SELECTION_SIZE		12

PlaceItem::PlaceItem(QPointF position, QString name) : NetItem(){
	this->setName(name);
	this->setPos(position);

	//Configure the underlying graphics item
	this->setFlag(QGraphicsItem::ItemIsMovable, true);
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}

int PlaceItem::type() const{
	return NetEntity::PlaceItem;
}

QRectF PlaceItem::boundingRect() const{
	QPainterPath path;
	path.addEllipse(QPointF(0,0), SELECTION_SIZE, SELECTION_SIZE);
	return path.controlPointRect();
}

QPainterPath PlaceItem::shape() const{
	QPainterPath path;
	path.addEllipse(QPointF(0,0), CIRCLE_SIZE, CIRCLE_SIZE);
	return path;
}

QPainterPath PlaceItem::opaqueArea() const{
	return shape();
}

QPointF PlaceItem::nearestPoint(QPointF to) const{
	QLineF line = QLineF(this->pos(), to);
	line = line.unitVector();
	line.setLength(CIRCLE_SIZE);
	return line.p2();
}

void PlaceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*){
	painter->drawEllipse(QPointF(0,0), CIRCLE_SIZE, CIRCLE_SIZE);
	if(this->isSelected()){
		painter->setBrush(Qt::NoBrush);
		QPen pen(Qt::DotLine);
		pen.setColor(QColor(0, 0, 0, 128));
		painter->setPen(pen);
		painter->drawEllipse(QPointF(0,0), SELECTION_SIZE, SELECTION_SIZE);
	}
}
