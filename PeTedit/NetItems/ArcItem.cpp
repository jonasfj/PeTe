#include "ArcItem.h"

#include <QPainter>
#include <QPainterPath>
#include <QTransform>
#include <QtGlobal>
#include <QDebug>

#define ARROW_SIZE	5

ArcItem::ArcItem(NetItem* start, NetItem* end){
	Q_ASSERT(start != NULL);
	_startItem = start;
	_endItem = end;
	setWeight(1);
	_startItem->registerConnectedItem(this);
	if(_endItem)
		_endItem->registerConnectedItem(this);
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);
	this->updateEndPoints();
}

int ArcItem::type() const{
	return NetEntity::ArcItem;
}

QRectF ArcItem::boundingRect() const{
	return shape().controlPointRect();
}

void ArcItem::updateTextPath() {
	QPainterPath path;
	if((_endItem && _startItem->collidesWithItem(_endItem)) ||
	   (!_endItem && _startItem->contains(_end)))
		return;
	QPointF start(0,0),
			point = _end - pos();

	//The arrow line and reverse liune
	QLineF line(start, point);
	QLineF revline(point, start);

	//Make some text
	if(this->weight() != 1){
		QFont font;
		font.setPointSizeF(6);
		path.addText(QPointF(0,0), font, QString::number(this->weight()));
		//Move it into some reasonable position
		path.translate(-path.boundingRect().width()/2, -3);
		QTransform rotation;
		qreal angle = line.angle();
		if(angle > 90 && angle < 270)
			angle = 180 - angle;
		else
			angle = 360 - angle;
		rotation.rotate(angle);

		path = rotation.map(path);
		path.translate(point/2);
	}
	_cachedTextPath = path;
}

QPainterPath ArcItem::shape() const{
	QPainterPath path;
	path.addPath(_cachedArrowPath);
	path.addPath(_cachedTextPath);
	return path;
}

QPainterPath ArcItem::opaqueArea() const{
	return shape();
}

void ArcItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*){
	painter->setBrush(Qt::SolidPattern);
	painter->drawPath(_cachedArrowPath);
	painter->setPen(Qt::NoPen);
	painter->drawPath(_cachedTextPath);
}

void ArcItem::updateEndPoints(){
	this->prepareGeometryChange();
	if(_endItem)
		_end = _endItem->nearestPoint(_startItem->pos());
	QPointF start = _startItem->nearestPoint(_end);
	this->setPos(start);
	if(_cachedPoint != _end - pos()){
		_cachedPoint = _end - pos();
		updateArrowPath();
		updateTextPath();
	}
}

void ArcItem::updateArrowPath(){
	QPainterPath path;
	if((_endItem && _startItem->primaryShape().intersects(_endItem->primaryShape())) ||
	   (!_endItem && _startItem->contains(_end))){
	   _cachedArrowPath = path;
		return;
	}
	QPointF start(0,0),
			point = _end - pos();

	//The arrow line and reverse liune
	QLineF revline(point, start);

	//Compute various points
	QLineF s = revline.normalVector();
	s.setAngle(revline.angle() - 45);
	s.setLength(ARROW_SIZE);
	QPointF side1 = s.p2();
	s = revline.normalVector();
	s.setAngle(revline.angle() + 45);
	s.setLength(ARROW_SIZE);
	QPointF side2 = s.p2();

	s = QLineF(side1, side2);
	QPointF head = point;
	s.intersect(revline, &head);

	path.moveTo(start);
	path.lineTo(head);
	path.lineTo(side1);
	path.lineTo(point);
	path.lineTo(side2);
	path.lineTo(head);

	_cachedArrowPath = path;
}

void ArcItem::setEndPoint(QPointF end){
	this->prepareGeometryChange();
	_end = end;
	QPointF start = _startItem->nearestPoint(_end);
	this->setPos(start);
}

void ArcItem::setEnd(NetItem* item){
	this->prepareGeometryChange();
	if(_endItem)
		_endItem->unregisterConnectedItem(this);
	_endItem = item;
	if(_endItem){
		_endItem->registerConnectedItem(this);
		_end = _endItem->nearestPoint(_startItem->pos());
	}
	QPointF start = _startItem->nearestPoint(_end);
	this->setPos(start);
}

void ArcItem::registerAtEndPoints(){
	if(_endItem)
		_endItem->registerConnectedItem(this);
	if(_startItem)
		_startItem->registerConnectedItem(this);
}

void ArcItem::unregisterAtEndPoints(){
	if(_endItem)
		_endItem->unregisterConnectedItem(this);
	if(_startItem)
		_startItem->unregisterConnectedItem(this);
}
