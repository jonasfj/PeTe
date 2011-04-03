#ifndef EDITQUERYCOMMAND_H
#define EDITQUERYCOMMAND_H

#include <QUndoCommand>
#include "../Misc/QueryModel.h"

/** Command that modifies a QueryItem */
class EditQueryCommand : public QUndoCommand
{
public:
	EditQueryCommand(QueryModel* model,
					 int row,
					 const QueryModel::Query& query);
	void undo();
	void redo();
private:
	void swap();
	QueryModel* _model;
	QueryModel::Query _query;
	int _row;
};

#endif // EDITQUERYCOMMAND_H
