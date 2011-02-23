#ifndef MOVEITEMSCOMMAND_H
#define MOVEITEMSCOMMAND_H

#include <QUndoCommand>
#include <QPointF>
#include <QGraphicsItem>

class MoveItemsCommand : public QUndoCommand
{
public:
	MoveItemsCommand(QList<QGraphicsItem*> items, qreal dx, qreal dy);
	void undo();
	void redo();
	int id() const;
	bool mergeWith(const QUndoCommand *other);
private:
	QList<QGraphicsItem*> _items;
	qreal _dx, _dy;
};

#endif // MOVEITEMSCOMMAND_H
