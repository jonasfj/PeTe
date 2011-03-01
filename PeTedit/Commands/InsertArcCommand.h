#ifndef INSERTARCCOMMAND_H
#define INSERTARCCOMMAND_H

#include <QUndoCommand>

class PetriNetScene;
class ArcItem;

class InsertArcCommand : public QUndoCommand
{
public:
	InsertArcCommand(PetriNetScene* scene, ArcItem* arc);
	~InsertArcCommand();
	void undo();
	void redo();
private:
	PetriNetScene* _scene;
	ArcItem* _arc;
	bool _removed;
};

#endif // INSERTARCCOMMAND_H
