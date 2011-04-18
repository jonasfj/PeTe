#ifndef ADDREMOVEQUERYCOMMAND_H
#define ADDREMOVEQUERYCOMMAND_H

#include <QUndoCommand>

#include "../Misc/QueryModel.h"

class QueryItem;

/** Command for adding or removing a QueryItem */
class AddRemoveQueryCommand : public QUndoCommand
{
public:
	/** Add new query */
	AddRemoveQueryCommand(QueryModel* model, const QueryModel::Query& query, QUndoCommand* parent = NULL);
	/** Remove query at row */
	AddRemoveQueryCommand(QueryModel* model, int row);
	~AddRemoveQueryCommand();
	void redo();
	void undo();
private:
	void swap();
	QueryModel* _model;
	QueryModel::Query _query;
	bool _add;
	int _row;
};

#endif // ADDREMOVEQUERYCOMMAND_H
