#ifndef INSERTTRANSITIONCOMMAND_H
#define INSERTTRANSITIONCOMMAND_H

#include <QUndoCommand>
#include <QPointF>

class PetriNetScene;
class TransitionItem;

/** Command to insert a transition, for undo/redo support */
class InsertTransitionCommand : public QUndoCommand
{
public:
	InsertTransitionCommand(PetriNetScene* scene, QPointF position);
	~InsertTransitionCommand();
	TransitionItem* transition() {return _transition;}
	void undo();
	void redo();
private:
	PetriNetScene* _scene;
	TransitionItem* _transition;
	bool _removed;
};


#endif // INSERTTRANSITIONCOMMAND_H
