#ifndef PETRINETSCENE_H
#define PETRINETSCENE_H

#include <QGraphicsScene>
#include <QUndoStack>
#include <QUndoGroup>
#include <QStandardItemModel>

#include "AbstractPetriNetFactory.h"

class NetItem;
class PlaceItem;
class TransitionItem;
class ArcItem;

/** A TAPN document, the model behind the view */
class PetriNetScene : public QGraphicsScene
{
    Q_OBJECT
public:
	/** Type for manageing scene mode */
	enum Mode {
		/** Pointer mode, where items can be modified and moved */
		PointerMode,
		/** Insert place mode, where a click inserts a new place */
		InsertPlaceMode,
		/** Insert transition mode, where a click inserts a new transition */
		InsertTransitionMode,
		/** Insert arc mode, where two clicks inserts a new arc */
		InsertArcMode
	};

	/** Create new instance of TapnScene
	 * @param undoGroup		UndoGroup to add undostack to
	 * @param parent		Widget to die, with unless killed otherwise.
	 */
	explicit PetriNetScene(QUndoGroup* undoGroup, QObject *parent = 0);

	/** Notify the document that it's active  */
	void setActive(){
		this->_undoStack->setActive();
	}

	Mode mode() {return _mode;}
	void setMode(Mode mode) {
		_mode = mode;
		emit modeChanged(mode);
	}

	/** Find a net item with the given name */
	NetItem* findNetItem(const QString& name);

	void addNetItem(NetItem* item);

	void removeNetItem(NetItem* item);

	/** Add new arc */
	void addArc(ArcItem* arc);
	/** Remove arc*/
	void removeArc(ArcItem* arc);
	/** Find an arc from start to end, NULL if none */
	ArcItem* findArc(NetItem* start, NetItem* end);

	/** Produce a copy using factory */
	void produce(PetriEngine::AbstractPetriNetFactory* factory);

	/** Add a new variable to the net */
	void addVariable(QString name, int value, int range);

	/** Get the variables */
	QStandardItemModel* variables() {
		return this->_variables;
	}

	/** Get query model */
	QStandardItemModel* queries() {
		return _queries;
	}
private:
	void transitionItemDoubleClickEvent(TransitionItem* t);
	void placeItemDoubleClickEvent(PlaceItem* place);
	void arcItemDoubleClickEvent(ArcItem* arc);

	/** Model representing the variables of the net */
	QStandardItemModel* _variables;

	/** Model holding queries */
	QStandardItemModel* _queries;

	/** Undo stack */
	QUndoStack* _undoStack;

	Mode _mode;
	/** Unselect item at mouseReleaseEvent if ControlModifier is down */
	bool unselectItemAtReleaseIfCtrlDown;
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void keyPressEvent(QKeyEvent* event);
signals:
	void modeChanged(PetriNetScene::Mode mode);
public slots:
	void updateSceneRect();
	/** Add new query */
	void addQuery();
	/** Remove query */
	void removeQuery(const QModelIndex& index);
	/** Edit query */
	void editQuery(const QModelIndex& index);
};

#endif // PETRINETSCENE_H
