#include "AddRemoveQueryCommand.h"

#include "../Misc/QueryModel.h"

AddRemoveQueryCommand::AddRemoveQueryCommand(QueryModel* model,
											 const QueryModel::Query& query,
											 bool add){
	_model = model;
	_query = query;
	_add = add;
	_index = -1;
}

AddRemoveQueryCommand::~AddRemoveQueryCommand(){}

void AddRemoveQueryCommand::redo(){
	swap();
}

void AddRemoveQueryCommand::undo(){
	swap();
}

void AddRemoveQueryCommand::swap(){
	if(_add){
		if(_index == -1)
			_index = _model->rowCount();
		_model->beginInsertRows(QModelIndex(), _index, _index);
		_model->_queries.insert(_index, _query);
		_model->endInsertRows();
	}else{
		_index = _model->_queries.indexOf(_query);
		Q_ASSERT(_index != -1);
		if(_index != -1){
			_model->beginRemoveRows(QModelIndex(), _index, _index);
			_model->_queries.removeAt(_index);
			_model->endRemoveRows();
		}
	}
	_add = !_add;
}
