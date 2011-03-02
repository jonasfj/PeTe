#include "ArcItem.h"
#include "../Commands/InsertPlaceCommand.h"
#include "../Commands/InsertTransitionCommand.h"
#include "../Commands/MoveItemsCommand.h"
#include "../Commands/InsertArcCommand.h"
#include "PlaceItem.h"
#include "TransitionItem.h"
// DIALOGS
#include "../Dialogs/EditArcDialog.h"
#include "../Dialogs/EditPlaceDialog.h"
#include "../Dialogs/EditTransitionDialog.h"

#include "PetriNetScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QtGlobal>
#include <QMessageBox>

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

NetItem* PetriNetScene::findNetItem(const QString &name){
	foreach(QGraphicsItem* item, this->items()){
		if(item->type() == NetEntity::PlaceItem || item->type() == NetEntity::TransitionItem){
			NetItem* i = dynamic_cast<NetItem*>(item);
			if(i->name() == name)
				return i;
		}
	}
	return NULL;
}

/******************** Add, remove and find transition ********************/

void PetriNetScene::addNetItem(NetItem* item){
	if(!this->items().contains(item))
		this->addItem(item);
	this->updateSceneRect();
}

void PetriNetScene::removeNetItem(NetItem* item){
	this->removeItem(item);
	this->updateSceneRect();
}


/******************** Add, remove and arc ********************/

void PetriNetScene::addArc(ArcItem* arc){
	if(!this->items().contains(arc))
		this->addItem(arc);
	this->updateSceneRect();
}

void PetriNetScene::removeArc(ArcItem* arc){
	this->removeItem(arc);
	this->updateSceneRect();
}

ArcItem* PetriNetScene::findArc(NetItem *start, NetItem *end){
	foreach(QGraphicsItem* item, this->items()){
		if(item->type() == NetEntity::ArcItem) {
			ArcItem* arc = dynamic_cast<ArcItem*>(item);

			if(arc->start() == start && arc->end() == end)
				return arc;
		}
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
					arc->unregisterAtEndPoints();
					removeItem(arc);
					delete arc;
				}
			}
		}
	}
}

void showMessageBox(QString text, QString infoText){
	QMessageBox msgBox;
	msgBox.setText(text);
	msgBox.setInformativeText(infoText);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.setIcon(QMessageBox::Information);
	msgBox.exec();
}

void PetriNetScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
	if(this->mode() == PointerMode){
		QGraphicsItem* item = itemAt(event->scenePos());
		if(item && item->type() == NetEntity::ArcItem){
			ArcItem* arc = dynamic_cast<ArcItem*>(item);
			//Lanuch a dialog an modify arc
			EditArcDialog* dlg = new EditArcDialog(dynamic_cast<QWidget*>(this->parent()));
			dlg->setInfo(tr("Edit arc from \"%1\" to \"%2\".").arg(arc->start()->name()).arg(arc->end()->name()));
			dlg->setWeight(arc->weight());
			if(dlg->exec() == QDialog::Accepted)
				arc->setWeight(dlg->weight());
			dlg->deleteLater();
		} else if( item && item->type() == NetEntity::PlaceItem){
			PlaceItem* place = dynamic_cast<PlaceItem*>(item);

			// Open dialog
			EditPlaceDialog* dlg = new EditPlaceDialog(dynamic_cast<QWidget*>(this->parent()));
			dlg->setName(place->name());
			dlg->setTokens(place->tokens());

			if(dlg->exec() == QDialog::Accepted){
				QString name = dlg->name().trimmed();
				if(!name.isEmpty() && name != place->name()){
					if(!this->findNetItem(name)){
						place->setName(name);
					} else {
						showMessageBox(tr("Place was not renamed"),
									   tr("Another item with the same name already exists. Please provide another name."));
					}
				}
				place->setTokens(dlg->tokens());
			}
			dlg->deleteLater();

		} else if(item && item->type() == NetEntity::TransitionItem){
			TransitionItem* t = dynamic_cast<TransitionItem*>(item);

			// Open transition edit dialog
			EditTransitionDialog* dlg = new EditTransitionDialog(dynamic_cast<QWidget*>(this->parent()));
			dlg->setName(t->name());
			if(dlg->exec()==QDialog::Accepted){
				QString name = dlg->name().trimmed();
				if(!name.isEmpty() && name != t->name()) {
					if(!this->findNetItem(name)){
						t->setName(name);
					} else {
						showMessageBox(tr("Transition was not renamed"),
									   tr("Another item with the same name already exists. Please provide another name."));
					}
				}
			}
			dlg->deleteLater();
		}
	}
}

/******************** Produce using factory ********************/

void PetriNetScene::produce(PetriEngine::AbstractPetriNetFactory* factory){
	foreach(QGraphicsItem* item, this->items()) {
		if(item->type() == NetEntity::PlaceItem) {
			PlaceItem* p = dynamic_cast<PlaceItem*>(item);
			Q_ASSERT(p != NULL);
			factory->addPlace(p->name().toStdString(), p->tokens(), p->pos().x(), p->pos().y());
		} else if (item->type() == NetEntity::TransitionItem){
			TransitionItem* t = dynamic_cast<TransitionItem*>(item);
			Q_ASSERT(t != NULL);
			factory->addTransition(t->name().toStdString(), t->pos().x(), t->pos().y());
		}
	}
	foreach(QGraphicsItem* item, this->items()){
		if(item->type() == NetEntity::ArcItem){
			ArcItem* a = dynamic_cast<ArcItem*>(item);
			Q_ASSERT(a != NULL);
			std::string start = a->start()->name().toStdString();
			std::string end = a->end()->name().toStdString();
			if(a->isInputArc())
				factory->addInputArc(start, end, a->weight());
			else
				factory->addOutputArc(start, end, a->weight());
		}
	}
}
