#include "arcitem.h"

#include <QPainter>
#include <QPainterPath>
#include <QtGlobal>
#include <QDebug>

ArcItem::ArcItem(NetItem* start, NetItem* end){
	_start = start;
	_end = end;
	_start->registerConnectedItem(this);
	_end->registerConnectedItem(this);
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}

int ArcItem::type() const{
	return NetEntity::ArcItem;
}

QRectF ArcItem::boundingRect() const{
	return shape().controlPointRect();
}

QPainterPath ArcItem::shape() const{
	QPainterPath path;
	if(_start->collidesWithItem(_end))
		return path;
	path.moveTo(_start->nearestPoint(_end->pos()));
	path.lineTo(_end->nearestPoint(_start->pos()));
	return path;
}

QPainterPath ArcItem::opaqueArea() const{
	return shape();
}

void ArcItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*){
	painter->drawPath(shape());
}

void ArcItem::updatePosition(){
	this->prepareGeometryChange();
}
