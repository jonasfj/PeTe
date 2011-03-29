#ifndef ADDREMOVEQUERYCOMMAND_H
#define ADDREMOVEQUERYCOMMAND_H

#include <QUndoCommand>

#include <QStandardItemModel>

class QueryItem;

/** Command for adding or removing a QueryItem */
class AddRemoveQueryCommand : public QUndoCommand
{
public:
	AddRemoveQueryCommand(QStandardItemModel* model, QueryItem* item, bool add);
	~AddRemoveQueryCommand();
	void redo();
	void undo();
private:
	void swap();
	QStandardItemModel* _model;
	QueryItem* _item;
	bool _add;
};

#endif // ADDREMOVEQUERYCOMMAND_H
