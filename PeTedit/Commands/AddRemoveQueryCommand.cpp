#include "AddRemoveQueryCommand.h"

#include "../Misc/QueryModel.h"

AddRemoveQueryCommand::AddRemoveQueryCommand(QueryModel* model,
											 const QueryModel::Query& query){
	_model = model;
	_query = query;
	_add = true;
	_row = -1;
}

AddRemoveQueryCommand::AddRemoveQueryCommand(QueryModel* model,
											 int row){
	_model = model;
	_add = false;
	_row = row;
}

AddRemoveQueryCommand::~AddRemoveQueryCommand(){}

void AddRemoveQueryCommand::redo(){
	swap();
}

void AddRemoveQueryCommand::undo(){
	swap();
}

void AddRemoveQueryCommand::swap(){
	if(_add)
		_row = _model->insertQuery(_query, _row);
	else{
		Q_ASSERT(_row != -1);
		_query = _model->takeQuery(_row);
	}
	_add = !_add;
}
