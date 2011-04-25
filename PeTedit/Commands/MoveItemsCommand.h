#ifndef MOVEITEMSCOMMAND_H
#define MOVEITEMSCOMMAND_H

#include <QUndoCommand>
#include <QPointF>
#include <QGraphicsItem>

class NetItem;

class MoveItemsCommand : public QUndoCommand
{
public:
	MoveItemsCommand(QList<NetItem*> items, qreal dx, qreal dy);
	void undo();
	void redo();
	int id() const;
	bool mergeWith(const QUndoCommand *other);
private:
	QList<NetItem*> _items;
	qreal _dx, _dy;
};

#endif // MOVEITEMSCOMMAND_H
