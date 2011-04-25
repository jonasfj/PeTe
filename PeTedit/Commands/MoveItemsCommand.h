#ifndef MOVEITEMSCOMMAND_H
#define MOVEITEMSCOMMAND_H

#include <QUndoCommand>
#include <QPointF>
#include <QGraphicsItem>

class PetriNetScene;
class NetItem;

class MoveItemsCommand : public QUndoCommand
{
public:
	MoveItemsCommand(PetriNetScene* scene, QList<NetItem*> items, qreal dx, qreal dy);
	void undo();
	void redo();
	int id() const;
	bool mergeWith(const QUndoCommand *other);
private:
	PetriNetScene* scene;
	QList<NetItem*> _items;
	qreal _dx, _dy;
};

#endif // MOVEITEMSCOMMAND_H
