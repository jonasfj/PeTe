#include "EditQueryCommand.h"

#include "../Misc/QueryItem.h"

EditQueryCommand::EditQueryCommand(QueryItem *item,
								   const QString &name,
								   const QString &query,
								   const QString &strategy){
	_item = item;
	_name = name;
	_query = query;
	_strategy = strategy;
}

void EditQueryCommand::undo(){
	swap();
}

void EditQueryCommand::redo(){
	swap();
}

void EditQueryCommand::swap(){
	QString name = _item->name(),
			query = _item->query(),
			strategy = _item->strategy();
	_item->setName(_name);
	_item->setQuery(_query);
	_item->setStrategy(_strategy);
	_name = name;
	_query = query;
	_strategy = strategy;
}
