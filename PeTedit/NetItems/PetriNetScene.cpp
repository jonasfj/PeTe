#include "ArcItem.h"
#include "../Commands/InsertPlaceCommand.h"
#include "../Commands/InsertTransitionCommand.h"
#include "../Commands/MoveItemsCommand.h"
#include "../Commands/InsertArcCommand.h"
#include "../Commands/RenameItemCommand.h"
#include "../Commands/EditPlaceCommand.h"
#include "../Commands/EditArcCommand.h"
#include "../Commands/DeleteItemCommand.h"
#include "../Commands/EditTransitionCommand.h"
#include "PlaceItem.h"
#include "TransitionItem.h"

#include "../Misc/QueryItem.h"
#include "../Dialogs/QueryDialog.h"
#include "../Commands/EditQueryCommand.h"
#include "../Commands/AddRemoveQueryCommand.h"

// DIALOGS
#include "../Dialogs/EditArcDialog.h"
#include "../Dialogs/EditPlaceDialog.h"
#include "../Dialogs/EditTransitionDialog.h"

#include "PetriNetScene.h"

#include "../Misc/ValidationIssuesModel.h"

#include <PetriEngine/ValidationBuilder.h>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QtGlobal>
#include <QMessageBox>
#include <QPainterPath>
#include <QDebug>


PetriNetScene::PetriNetScene(QUndoGroup* undoGroup, QObject* parent) :
    QGraphicsScene(parent)
{
	//Create undostack for this document
	this->_undoStack = new QUndoStack(this);
	undoGroup->addStack(this->_undoStack);

	// Create variable model (name,value,range)
	this->_variables = new QStandardItemModel(0, 3, this);
	this->_variables->setHeaderData(0, Qt::Horizontal, "Name");
	this->_variables->setHeaderData(1, Qt::Horizontal, "Value");
	this->_variables->setHeaderData(2, Qt::Horizontal, "Range");

	// Create query model (name)
	this->_queries = new QStandardItemModel(0, 1, this);
	this->_queries->setHeaderData(0, Qt::Horizontal, "Query");

	// Create valiation issue model
	this->_validationIssues = new ValidationIssuesModel(this);

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

/** Check if this is a valid available identifier */
bool PetriNetScene::isValidAvailableIdentifier(const QString &id) const{
	QRegExp reg("^(?!\\d)\\w+$");
	if(!reg.exactMatch(id))
		return false;
	foreach(QGraphicsItem* item, this->items()){
		if(item->type() == NetEntity::PlaceItem || item->type() == NetEntity::TransitionItem){
			NetItem* i = dynamic_cast<NetItem*>(item);
			if(i->name() == id)
				return false;
		}
	}
	foreach(const QString& name, variableNames())
		if(name == id) return false;
	return true;
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

QStringList PetriNetScene::placeNames() const{
	QStringList names;
	foreach(QGraphicsItem* item, this->items()){
		if(item->type() == NetEntity::PlaceItem){
			NetItem* i = dynamic_cast<NetItem*>(item);
			names.append(i->name());
		}
	}
	return names;
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

/******************** Variables ********************/

QStringList PetriNetScene::variableNames() const{
	QStringList list;
	for(int row = 0; row < this->_variables->rowCount(); row++)
		list.append(this->_variables->data(this->_variables->index(row, 0)).toString());
	return list;
}

/** Append a new variable to the list of variables */
void PetriNetScene::addVariable(QString name, int value, int range){
	QList<QStandardItem*> row;
	row.append( new QStandardItem());
	row.append( new QStandardItem());
	row.append( new QStandardItem());
	row.at(0)->setData(name,Qt::EditRole);
	row.at(1)->setData(value,Qt::EditRole);
	row.at(2)->setData(range,Qt::EditRole);

	this->_variables->appendRow(row);
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
		if(!item){
			QPainterPath path;
			path.addEllipse(event->scenePos(),5,5);
			QList<QGraphicsItem*> gi = items(path,Qt::IntersectsItemShape);
			if(!gi.isEmpty()){
				qDebug()<<"Was here!";
				item = gi.first();}
		}

		if(item && item->type() == NetEntity::ArcItem){
			ArcItem* arc = dynamic_cast<ArcItem*>(item);
			arcItemDoubleClickEvent(arc);
		} else if( item && item->type() == NetEntity::PlaceItem){
			PlaceItem* place = dynamic_cast<PlaceItem*>(item);
			this->placeItemDoubleClickEvent(place);
		} else if(item && item->type() == NetEntity::TransitionItem){
			TransitionItem* t = dynamic_cast<TransitionItem*>(item);
			this->transitionItemDoubleClickEvent(t);
		}
	}
}

/********************** Double click methods **********************/

/** Handle all double click events on ArcItems */
void PetriNetScene::arcItemDoubleClickEvent(ArcItem *arc){
	//Lanuch a dialog an modify arc
	EditArcDialog* dlg = new EditArcDialog(dynamic_cast<QWidget*>(this->parent()));
	dlg->setInfo(tr("Edit arc from \"%1\" to \"%2\".").arg(arc->start()->name()).arg(arc->end()->name()));
	dlg->setWeight(arc->weight());
	if(dlg->exec() == QDialog::Accepted)
		_undoStack->push(new EditArcCommand(arc, dlg->weight()));
	dlg->deleteLater();
}

/** Handle all double click events on PlaceItems */
void PetriNetScene::placeItemDoubleClickEvent(PlaceItem *place){
	// Open dialog
	EditPlaceDialog* dlg = new EditPlaceDialog(dynamic_cast<QWidget*>(this->parent()));
	dlg->setName(place->name());
	dlg->setTokens(place->tokens());

	if(dlg->exec() == QDialog::Accepted){
		QString name = dlg->name().trimmed();
		int tokens = dlg->tokens();
		if(!name.isEmpty() && name != place->name()){
			if(isValidAvailableIdentifier(name)){
				_undoStack->push(new EditPlaceCommand(place, name, dlg->tokens()));
			} else {
				showMessageBox(tr("Place was not renamed"),
							   tr("Another item with the same name already exists. Please provide another name."));
			}
		} else if(!name.isEmpty() && tokens != place->tokens())
			_undoStack->push(new EditPlaceCommand(place, place->name(), tokens));
	}
	dlg->deleteLater();
}

/** Handle all double click events on TransitionItems */
void PetriNetScene::transitionItemDoubleClickEvent(TransitionItem *t){
	// Open transition edit dialog
	EditTransitionDialog* dlg = new EditTransitionDialog(dynamic_cast<QWidget*>(this->parent()));
	dlg->setName(t->name());
	dlg->setIdentifiers(placeNames(), variableNames());
	dlg->setPreConditions(t->preConditions());
	dlg->setPostConditions(t->postConditions());
	if(dlg->exec()==QDialog::Accepted){
		QString name = dlg->name().trimmed();
		QString preconds = dlg->preConditions();
		QString postconds = dlg->postConditions();

		//TODO: Attempt to parse conditions and push to undo stack.
		if((!name.isEmpty() && name != t->name()) || preconds != t->preConditions() || postconds != t->postConditions()){
			bool applychanges = true;
			if(name != t->name()){
				if(this->findNetItem(name)){
					showMessageBox(tr("Transition was not renamed"),
								   tr("Another item with the same name already exists. Please provide another name."));
					applychanges = false;
				}
			}
			if(applychanges)
				_undoStack->push(new EditTransitionCommand(t,name,preconds,postconds));
		}
	}
	dlg->deleteLater();
}



/******************** Key press events **********************/

void PetriNetScene::keyPressEvent(QKeyEvent *event) {
	if(event->key() == Qt::Key_Delete){
		//Iterate over selected items, and delete them
		foreach(QGraphicsItem* item, this->selectedItems()){
			if(item->type() == NetEntity::PlaceItem || item->type() == NetEntity::TransitionItem)
				_undoStack->push(new DeleteItemCommand(this, (NetItem*)item));
		}
	}
}

/******************** Produce using builder ********************/

void PetriNetScene::produce(PetriEngine::AbstractPetriNetBuilder* builder){

	for(int row = 0; row < this->_variables->rowCount(); row++){
		QString name = this->_variables->data(this->_variables->index(row, 0)).toString();
		int value = this->_variables->data(this->_variables->index(row, 1)).toInt();
		int range = this->_variables->data(this->_variables->index(row, 2)).toInt();
		builder->addVariable(name.toStdString(), value, range);
	}

	foreach(QGraphicsItem* item, this->items()) {
		if(item->type() == NetEntity::PlaceItem) {
			PlaceItem* p = dynamic_cast<PlaceItem*>(item);
			Q_ASSERT(p != NULL);
			builder->addPlace(p->name().toStdString(), p->tokens(), p->pos().x(), p->pos().y());
		} else if (item->type() == NetEntity::TransitionItem){
			TransitionItem* t = dynamic_cast<TransitionItem*>(item);
			Q_ASSERT(t != NULL);
			builder->addTransition(t->name().toStdString(),
								   t->preConditions().toStdString(),
								   t->postConditions().toStdString(),
								   t->pos().x(),
								   t->pos().y());
		}
	}
	foreach(QGraphicsItem* item, this->items()){
		if(item->type() == NetEntity::ArcItem){
			ArcItem* a = dynamic_cast<ArcItem*>(item);
			Q_ASSERT(a != NULL);
			std::string start = a->start()->name().toStdString();
			std::string end = a->end()->name().toStdString();
			if(a->isInputArc())
				builder->addInputArc(start, end, a->weight());
			else
				builder->addOutputArc(start, end, a->weight());
		}
	}
}

/******************** Query model handling ********************/

void PetriNetScene::addQuery(){
	QStringList places;

	QueryItem* item = new QueryItem();
	QueryDialog d(item, dynamic_cast<QWidget*>(this->parent()));
	d.setIdentifiers(placeNames(), variableNames());
	if(d.exec() == QDialog::Accepted){
		item->setName(d.name());
		item->setQuery(d.query());
		item->setStrategy(d.strategy());
		AddRemoveQueryCommand* cmd = new AddRemoveQueryCommand(_queries, item, true);
		_undoStack->push(cmd);
	}else{
		delete item;
		item = NULL;
	}
}

void PetriNetScene::editQuery(const QModelIndex& index){
	QueryItem* item = dynamic_cast<QueryItem*>(_queries->itemFromIndex(index));
	if(!item) return;

	QueryDialog d(item, dynamic_cast<QWidget*>(this->parent()));
	d.setIdentifiers(placeNames(), variableNames());
	if(d.exec() == QDialog::Accepted){
		EditQueryCommand* cmd = new EditQueryCommand(item,
													 d.name(),
													 d.query(),
													 d.strategy());
		_undoStack->push(cmd);
	}
}

void PetriNetScene::removeQuery(const QModelIndex& index){
	QueryItem* item = dynamic_cast<QueryItem*>(_queries->itemFromIndex(index));
	if(!item) return;
	AddRemoveQueryCommand* cmd = new AddRemoveQueryCommand(_queries, item, false);
	_undoStack->push(cmd);
}


/******************** Validation handling ********************/

/** Refresh the model with validation issues */
void PetriNetScene::validate(){
	//Get the list of issues
	PetriEngine::ValidationBuilder validator;
	produce(&validator);
	validator.validate();

	_validationIssues->setIssues(validator.errors());

	//Clear existing issues
//	_validationIssues->clear();

	//List all the issues
	/*const std::vector<PetriEngine::ValidationError>& issues = validator.errors();
	for(size_t i = 0; issues.size(); i++){
		const PetriEngine::ValidationError& issue = issues[i];

		QString msg, location;
		if(issue.hasExprError()){
			msg = issue.exprError().toString().c_str();
			if(issue.isConditionError())
				location = "In the pre-condition for \"";
			else
				location = "In the post-assignemnt for \"";
			location += issue.startIdentifier().c_str();
			location += "\"";
		}else{
			msg = issue.text().c_str();
			if(issue.endIdentifier().empty()){
				location += "In \"";
				location +=  issue.startIdentifier().c_str();
				location += "\"";
			}else{
				location = "In arc from \"";
				location += issue.startIdentifier().c_str();
				location += "\" to \"";
				location += issue.endIdentifier().c_str();
				location += "\"";
			}
		}

		QStandardItem* issueItem = new QStandardItem(msg);
		QStandardItem* locationItem = new QStandardItem(location);
		QList<QStandardItem*> row;
		row.append(issueItem);
		row.append(locationItem);
		_validationIssues->appendRow(row);
	}*/
}

void PetriNetScene::showValidationIssue(const QModelIndex &index){
	_validationIssues->showValidationIssue(this, index);
}
