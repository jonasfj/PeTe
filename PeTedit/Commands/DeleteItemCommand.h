#ifndef DELETEITEMCOMMAND_H
#define DELETEITEMCOMMAND_H

#include <QUndoCommand>
#include <QtGlobal>
#include <QList>

class PetriNetScene;
class NetItem;
class ArcItem;

class DeleteItemCommand : public QUndoCommand {
public:
	DeleteItemCommand(PetriNetScene* scene, NetItem* item, QUndoCommand* parent = NULL);
	void undo();
	void redo();
private:
	PetriNetScene* _scene;
	NetItem* _item;
	QList<ArcItem*> _arcs;
};

#endif // DELETEITEMCOMMAND_H
