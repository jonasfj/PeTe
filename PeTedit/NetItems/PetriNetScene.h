#ifndef PETRINETSCENE_H
#define PETRINETSCENE_H

#include <QGraphicsScene>
#include <QUndoStack>
#include <QUndoGroup>
#include <QStandardItemModel>

#include <PetriEngine/AbstractPetriNetBuilder.h>

class NetItem;
class PlaceItem;
class TransitionItem;
class ArcItem;
class ValidationIssuesModel;
class QueryModel;
class PNMLBuilder;
class VariableModel;

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

	/** Get the undo stack for this scene */
	QUndoStack* undoStack() { return _undoStack; }

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

	/** Produce a copy using builder */
	void produce(PetriEngine::AbstractPetriNetBuilder* builder);
	/** Save, this extension method will also save queries */
	void produce(PNMLBuilder* builder);

	/** Get the variables */
	VariableModel* variables() { return this->_variables; }

	/** Get query model */
	QueryModel* queries() { return _queries; }

	/** Get validation issue model */
	ValidationIssuesModel* validationIssues() { return _validationIssues; }

	/** Check if this is a valid available identifier */
	bool isValidAvailableIdentifier(const QString& id) const;

	/** List variables names */
	QStringList variableNames() const;
	QStringList placeNames() const;
private:
	void transitionItemDoubleClickEvent(TransitionItem* t);
	void placeItemDoubleClickEvent(PlaceItem* place);
	void arcItemDoubleClickEvent(ArcItem* arc);

	/** Model representing the variables of the net */
	VariableModel* _variables;

	/** Model holding queries */
	QueryModel* _queries;

	/** Model holding validation issues, from last call to validate */
	ValidationIssuesModel* _validationIssues;

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
	void validationIssuesFound();
public slots:
	void updateSceneRect();
	/** Refresh the model with validation issues */
	void validate();
	/** Scroll to, select or otherwise highlight the issue */
	void showValidationIssue(const QModelIndex& index);
};

#endif // PETRINETSCENE_H
