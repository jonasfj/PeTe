#ifndef PETRINETSCENE_H
#define PETRINETSCENE_H

#include <QGraphicsScene>
#include <QUndoStack>
#include <QUndoGroup>

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

	/** Add new place */
	void addPlace(PlaceItem* place);
	/** Remove place */
	void removePlace(PlaceItem* place);
	/** Find a place given it's name */
	PlaceItem* findPlace(const QString& name);

	/** Add new transition */
	void addTransition(TransitionItem* transition);
	/** Remove transition */
	void removeTransition(TransitionItem* transition);
	/** Find a transition given it's name */
	TransitionItem* findTransition(const QString& name);

	/** Add new arc */
	void addArc(ArcItem* arc);
	/** Remove arc*/
	void removeArc(ArcItem* arc);
	/** Find an arc from start to end, NULL if none */
	ArcItem* findArc(NetItem* start, NetItem* end);

	/** Produce a copy using factory */
	void produce(PetriEngine::AbstractPetriNetFactory* factory);

	QUndoStack* _undoStack;
private:
	Mode _mode;
	/** List of all places */
	QList<PlaceItem*> places;
	QList<TransitionItem*> transitions;
	QList<ArcItem*> arcs;
	/** Unselect item at mouseReleaseEvent if ControlModifier is down */
	bool unselectItemAtReleaseIfCtrlDown;
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
signals:
	void modeChanged(PetriNetScene::Mode mode);
public slots:
	void updateSceneRect();
};

#endif // PETRINETSCENE_H
