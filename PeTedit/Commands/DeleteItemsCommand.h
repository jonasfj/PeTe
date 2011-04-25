#ifndef DELETEITEMSCOMMAND_H
#define DELETEITEMSCOMMAND_H

#include <QUndoCommand>
#include <QtGlobal>
#include <QList>

class PetriNetScene;
class NetItem;
class ArcItem;

class DeleteItemsCommand : public QUndoCommand {
public:
	DeleteItemsCommand(PetriNetScene* scene, QList<NetItem*> items, QList<ArcItem*> arcs, QUndoCommand* parent = NULL);
	void undo();
	void redo();
	~DeleteItemsCommand();
private:
	PetriNetScene* _scene;
	QList<NetItem*> _items;
	QList<ArcItem*> _arcs;
	bool added;
};

#endif // DELETEITEMSCOMMAND_H
