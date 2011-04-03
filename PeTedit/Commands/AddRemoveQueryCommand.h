#ifndef ADDREMOVEQUERYCOMMAND_H
#define ADDREMOVEQUERYCOMMAND_H

#include <QUndoCommand>

#include "../Misc/QueryModel.h"

class QueryItem;

/** Command for adding or removing a QueryItem */
class AddRemoveQueryCommand : public QUndoCommand
{
public:
	AddRemoveQueryCommand(QueryModel* model, const QueryModel::Query& query, bool add);
	~AddRemoveQueryCommand();
	void redo();
	void undo();
private:
	void swap();
	QueryModel* _model;
	QueryModel::Query _query;
	bool _add;
	int _index;
};

#endif // ADDREMOVEQUERYCOMMAND_H
