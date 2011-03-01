#include "ArcItem.h"
#include "../Commands/InsertPlaceCommand.h"
#include "../Commands/InsertTransitionCommand.h"
#include "../Commands/MoveItemsCommand.h"
#include "../Commands/InsertArcCommand.h"
#include "PlaceItem.h"
#include "TransitionItem.h"
#include "../Dialogs/EditArcDialog.h"

#include "PetriNetScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QtGlobal>

PetriNetScene::PetriNetScene(QUndoGroup* undoGroup, QObject* parent) :
    QGraphicsScene(parent)
{
	//Create undostack for this document
	this->_undoStack = new QUndoStack(this);
	undoGroup->addStack(this->_undoStack);

	//Set initial mode
	this->setMode(PointerMode);
	this->setSceneRect(0, 0, 1, 1);
}

void PetriNetScene::updateSceneRect(){
	QRectF rect = this->itemsBoundingRect();
	if(rect.x() > 0)
		rect.setX(0);
	if(rect.y() > 0)
		rect.setY(0);
	rect.adjust(0, 0, 50, 50);
	this->setSceneRect(rect);
}

/******************** Add, remove and find place ********************/

void PetriNetScene::addPlace(PlaceItem* place){
	if(!this->places.contains(place))
		this->places.append(place);
	this->addItem(place);
	this->updateSceneRect();
}

void PetriNetScene::removePlace(PlaceItem* place){
	this->places.removeAll(place);
	this->removeItem(place);
	this->updateSceneRect();
}

PlaceItem* PetriNetScene::findPlace(const QString& name){
	foreach(PlaceItem* place, this->places){
		if(place->name() == name)
			return place;
	}
	return NULL;
}

/******************** Add, remove and find transition ********************/

void PetriNetScene::addTransition(TransitionItem* transition){
	if(!this->transitions.contains(transition))
		this->transitions.append(transition);
	this->addItem(transition);
	this->updateSceneRect();
}

void PetriNetScene::removeTransition(TransitionItem* transition){
	this->transitions.removeAll(transition);
	this->removeItem(transition);
	this->updateSceneRect();
}

TransitionItem* PetriNetScene::findTransition(const QString& name){
	foreach(TransitionItem* transition, this->transitions){
		if(transition->name() == name)
			return transition;
	}
	return NULL;
}


/******************** Add, remove and arc ********************/

void PetriNetScene::addArc(ArcItem* arc){
	if(!this->arcs.contains(arc))
		this->arcs.append(arc);
	this->addItem(arc);
	this->updateSceneRect();
}

void PetriNetScene::removeArc(ArcItem* arc){
	this->arcs.removeAll(arc);
	this->removeItem(arc);
	this->updateSceneRect();
}

ArcItem* PetriNetScene::findArc(NetItem *start, NetItem *end){
	foreach(ArcItem* arc, this->arcs){
		if(arc->start() == start && arc->end() == end)
			return arc;
	}
	return NULL;
}

/******************** Mouse event handling ********************/

void PetriNetScene::mousePressEvent(QGraphicsSceneMouseEvent* event){
	if(this->mode() == InsertPlaceMode && event->button() == Qt::LeftButton){
		InsertPlaceCommand* cmd = new InsertPlaceCommand(this, event->scenePos());
		PlaceItem* place = cmd->place();
		this->_undoStack->push(cmd);
		this->setMode(PointerMode);
		this->clearSelection();
		place->setSelected(true);
		place->setFocus(Qt::MouseFocusReason);
	}else if(this->mode() == InsertTransitionMode && event->button() == Qt::LeftButton){
		InsertTransitionCommand* cmd = new InsertTransitionCommand(this, event->scenePos());
		TransitionItem* transition = cmd->transition();
		this->_undoStack->push(cmd);
		this->setMode(PointerMode);
		this->clearSelection();
		transition->setSelected(true);
		transition->setFocus(Qt::MouseFocusReason);
	}else if(this->mode() == InsertArcMode && event->button() == Qt::LeftButton){
		//Insert arc an selected it
		clearSelection();
		NetItem* start = dynamic_cast<NetItem*>(itemAt(event->scenePos()));
		if(start){
			ArcItem* arc = new ArcItem(start);
			arc->setEndPoint(start->pos());
			this->addItem(arc);
			arc->setSelected(true);
		}
	}else if(this->mode() == PointerMode && event->button() == Qt::LeftButton){
		//Assume PointerMode
		QGraphicsItem* item = itemAt(event->scenePos());

		if(item){
			this->unselectItemAtReleaseIfCtrlDown = false;
			bool state = item->isSelected();
			if(event->modifiers() & Qt::ShiftModifier){
				state = true;
			}else if(event->modifiers() & Qt::ControlModifier){
				//This should be done in releaseEvent, but that's hard...
				this->unselectItemAtReleaseIfCtrlDown = state;
				state = true;
			}else if(!state){
				state = true;
				this->clearSelection();
			}
			item->setSelected(state);
			if(state)
				item->setFocus(Qt::MouseFocusReason);
		}else{
			//If neither shift or ctrl is down clear selection
			if(!(event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier)))
				this->clearSelection();

			//TODO: Initiate selection rectangle...
		}
	}
}

void PetriNetScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event){
	if(this->mode() == PointerMode && event->buttons() & Qt::LeftButton){
		QPointF d = event->scenePos() - event->lastScenePos();
		foreach(QGraphicsItem* item, this->selectedItems())
			item->moveBy(d.x(), d.y());
	}else if(this->mode() == InsertArcMode && event->buttons() & Qt::LeftButton){
		//Move end point of selected arc
		if(!selectedItems().isEmpty()){
			ArcItem* arc = dynamic_cast<ArcItem*>(selectedItems().first());
			if(arc){
				NetItem* end = dynamic_cast<NetItem*>(itemAt(event->scenePos()));
				if(end && end->type() != arc->start()->type() && !findArc(arc->start(), end)){
					arc->setEndPoint(end->nearestPoint(arc->start()->pos()));
				}else
					arc->setEndPoint(event->scenePos());
			}
		}
	}
}

void PetriNetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event){
	if(this->mode() == PointerMode && event->button() == Qt::LeftButton){
		QPointF d = event->scenePos() - event->buttonDownScenePos(Qt::LeftButton);
		if(this->selectedItems().count() > 0 && (d.x() != 0 && d.y() != 0)){
			//Move back to start so we can apply the MoveItemsCommand
			//Slightly nasty, but it doesn't redraw here so this isn't bad.
			foreach(QGraphicsItem* item, this->selectedItems())
				item->moveBy(-d.x(), -d.y());
			_undoStack->push(new MoveItemsCommand(this->selectedItems(), d.x(), d.y()));

			this->updateSceneRect();
		}

		//Unselect if needed
		if(event->modifiers() & Qt::ControlModifier && this->unselectItemAtReleaseIfCtrlDown){
			QGraphicsItem* item = itemAt(event->scenePos());
			if(item && item->isSelected())
				item->setSelected(false);
		}
	}else if(this->mode() == InsertArcMode && event->button() == Qt::LeftButton){
		//Move end point of selected arc
		if(!selectedItems().isEmpty()){
			ArcItem* arc = dynamic_cast<ArcItem*>(selectedItems().first());
			if(arc){
				NetItem* end = dynamic_cast<NetItem*>(itemAt(event->scenePos()));
				if(end && end->type() != arc->start()->type() && !findArc(arc->start(), end)){
					arc->setEnd(end);
					removeItem(arc);
					InsertArcCommand* a = new InsertArcCommand(this, arc);
					_undoStack->push(a);
					this->setMode(PointerMode);
				}else{
					removeItem(arc);
					delete arc;
				}
			}
		}
	}
}

void PetriNetScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
	if(this->mode() == PointerMode){
		QGraphicsItem* item = itemAt(event->scenePos());
		if(item && item->type() == NetEntity::ArcItem){
			ArcItem* arc = dynamic_cast<ArcItem*>(item);
			//Lanuch a dialog an modify arc
			EditArcDialog* dlg = new EditArcDialog(dynamic_cast<QWidget*>(this->parent()));
			dlg->setInfo(tr("Edit arc from %1 to %2.").arg(arc->start()->name()).arg(arc->start()->name()));
			dlg->setWeight(arc->weight());
			if(dlg->exec() == QDialog::Accepted)
				arc->setWeight(dlg->weight());
			dlg->deleteLater();
		}
	}
}

/******************** Produce using factory ********************/

void PetriNetScene::produce(PetriEngine::AbstractPetriNetFactory* factory){
	foreach(PlaceItem* p, places)
		factory->addPlace(p->name().toStdString(), p->pos().x(), p->pos().y());
	foreach(TransitionItem* t, transitions)
		factory->addTransition(t->name().toStdString(), t->pos().x(), t->pos().y());
	foreach(ArcItem* a, arcs){
		std::string start = a->start()->name().toStdString();
		std::string end = a->end()->name().toStdString();
		if(a->isInputArc())
			factory->addInputArc(start, end, 1);
		else
			factory->addOutputArc(start, end, 1);
	}
}
