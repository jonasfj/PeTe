#include "EditQueryCommand.h"

#include "../Misc/QueryModel.h"

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
	QueryModel::Query q = _model->query(_row);
	_model->setQuery(_query, _row);
	_query = q;
}
