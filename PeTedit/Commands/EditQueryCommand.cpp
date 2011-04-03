#include "EditQueryCommand.h"

#include "../Misc/QueryItem.h"

EditQueryCommand::EditQueryCommand(QueryModel *model,
								   int row,
								   const QueryModel::Query& query){
	_model = model;
	_row = row;
	_query = query;
}

void EditQueryCommand::undo(){
	swap();
}

void EditQueryCommand::redo(){
	swap();
}

void EditQueryCommand::swap(){
	//TODO: Try emitting the dataChanged signal on QueryModel instead
	_model->beginResetModel();
	QueryModel::Query& q = _model->_queries[_row];
	_model->_queries[_row] = _query;
	_query = q;
	_model->endResetModel();
}
