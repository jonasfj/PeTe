#ifndef INSERTPLACECOMMAND_H
#define INSERTPLACECOMMAND_H

#include <QUndoCommand>
#include <QPointF>

class PetriNetScene;
class PlaceItem;

/** Command to insert a place, for undo/redo support */
class InsertPlaceCommand : public QUndoCommand
{
public:
	InsertPlaceCommand(PetriNetScene* scene, QPointF position);
	~InsertPlaceCommand();
	PlaceItem* place() {return _place;}
	void undo();
	void redo();
private:
	PetriNetScene* _scene;
	PlaceItem* _place;
	bool _removed;
};

#endif // INSERTPLACECOMMAND_H
