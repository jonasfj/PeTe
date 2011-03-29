#include "AddRemoveQueryCommand.h"

#include "../Misc/QueryItem.h"

AddRemoveQueryCommand::AddRemoveQueryCommand(QStandardItemModel *model,
											 QueryItem *item,
											 bool add){
	_model = model;
	_item = item;
	_add = add;
}

AddRemoveQueryCommand::~AddRemoveQueryCommand(){
	if(_add && _item)
		delete _item;
	_item = NULL;
}

void AddRemoveQueryCommand::redo(){
	swap();
}

void AddRemoveQueryCommand::undo(){
	swap();
}

void AddRemoveQueryCommand::swap(){
	if(_add){
		_model->appendRow(_item);
	}else{
		QModelIndex i = _model->indexFromItem(_item);
		QStandardItem* taken = _model->takeItem(i.row(), i.column());
		Q_ASSERT(taken == _item);
	}
	_add = !_add;
}
