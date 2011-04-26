#include "PlaceItem.h"
#include "ArcItem.h"
#include "TransitionItem.h"

#include <QPen>
#include <QBrush>
#include <QColor>

#include "NetItem.h"
#include <math.h>

#define CIRCLE_SIZE			7.5
#define SELECTION_SIZE		12
#define TOKEN_PADDING		3

PlaceItem::PlaceItem(QPointF position, QString name) : NetItem(){
	this->setName(name);
	this->setPos(position);
	this->setTokens(0);
	//Configure the underlying graphics item
	this->setFlag(QGraphicsItem::ItemIsMovable, true);
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}

int PlaceItem::type() const{
	return NetEntity::PlaceItem;
}

void PlaceItem::setTokens(int tokens){
	this->_tokens = tokens;
	// Update all output arcs (for enabledness)
	foreach(ArcItem* arc, this->ConnectedItems()){
		// If it is an output arc, notify the transition
		if(arc->end() != this)
			arc->end()->update();
	}
	updateTokenPath();
}

void PlaceItem::updateTokenPath(){
	// Centering number of tokens to place
	QPainterPath path;
	if(_tokens != 0){
		QFont f("",6);
		f.setBold(false);
		f.setStyleHint(QFont::SansSerif, QFont::NoAntialias);
		path.addText(0, 0, f, QString::number(this->_tokens));
		QTransform t;
		qreal width = path.boundingRect().width();
		qreal height = path.boundingRect().height();
		qreal s = ((CIRCLE_SIZE*2) - TOKEN_PADDING)/ sqrt(width*width + height*height);
		t.scale(s,s);
		path = t.map(path);
		QRectF bb = path.boundingRect();
		bb.moveCenter(QPointF(0,0));
		path.translate(bb.topLeft()-path.boundingRect().topLeft());
	}
	tokenPath = path;
}

void PlaceItem::updateTextPath(){
	QPainterPath path;

	// Draw text label for place
	QFont f("",6);
	f.setBold(false);
	f.setStyleHint(QFont::SansSerif, QFont::NoAntialias);
	path.addText(0,0,f,this->name());

	QRectF moved = path.boundingRect();
	qreal yoffset = path.boundingRect().height();
	moved.moveCenter(QPointF(0,yoffset*2 + 2));
	path.translate(moved.center() - path.boundingRect().center());
	textPath = path;
}

QRectF PlaceItem::boundingRect() const{
	QPainterPath path;
	path.addEllipse(QPointF(0,0), SELECTION_SIZE, SELECTION_SIZE);
	path.addPath(textPath);
	return path.controlPointRect();
}

QPainterPath PlaceItem::shape() const{
	QPainterPath path;
	path.addEllipse(QPointF(0,0), CIRCLE_SIZE, CIRCLE_SIZE);
	path.addPath(textPath);
	return path;
}

QPainterPath PlaceItem::opaqueArea() const{
	return shape();
}

void PlaceItem::nameChanged(){
	updateTextPath();
}

QPointF PlaceItem::nearestPoint(QPointF to) const{
	QLineF line = QLineF(this->pos(), to);
	if(line.length() == 0)	//Avoid bugs with NaN
		return to;
	line = line.unitVector();
	line.setLength(CIRCLE_SIZE);
	return line.p2();
}

void PlaceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*){
	painter->drawEllipse(QPointF(0,0), CIRCLE_SIZE, CIRCLE_SIZE);

	painter->setBrush(Qt::SolidPattern);
	painter->setPen(Qt::NoPen);
	painter->drawPath(tokenPath);

	// Draw place name
	painter->drawPath(textPath);
	if(this->isSelected()){
		painter->setBrush(Qt::NoBrush);
		QPen pen(Qt::DotLine);
		pen.setColor(QColor(0, 0, 0, 128));
		painter->setPen(pen);
		painter->drawEllipse(QPointF(0,0), SELECTION_SIZE, SELECTION_SIZE);
	}
}

QPainterPath PlaceItem::primaryShape(){
	QPainterPath path;
	path.addEllipse(QPointF(0,0), CIRCLE_SIZE, CIRCLE_SIZE);
	path.translate(pos());
	return path;
}
