#ifndef EDITQUERYCOMMAND_H
#define EDITQUERYCOMMAND_H

#include <QUndoCommand>

class QueryItem;

/** Command that modifies a QueryItem */
class EditQueryCommand : public QUndoCommand
{
public:
	EditQueryCommand(QueryItem* item,
					 const QString& name,
					 const QString& query,
					 const QString& strategy);
	void undo();
	void redo();
private:
	void swap();
	QueryItem* _item;
	QString _name, _query, _strategy;
};

#endif // EDITQUERYCOMMAND_H
