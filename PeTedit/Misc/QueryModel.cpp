#include "QueryModel.h"

#include "../NetItems/PetriNetScene.h"
#include "../Commands/AddRemoveQueryCommand.h"
#include "../Commands/EditQueryCommand.h"
#include "../Dialogs/QueryDialog.h"

#include <QIcon>

QueryModel::QueryModel(PetriNetScene* net)
	 : QAbstractTableModel(net){
	_net = net;
}

int QueryModel::rowCount(const QModelIndex &parent) const{
	if(parent.isValid())
		return 0;
	return _queries.count();
}

int QueryModel::columnCount(const QModelIndex &parent) const{
	if(parent.isValid())
		return 0;
	return 3;	// Icon, Name, Progress
}

QVariant QueryModel::data(const QModelIndex &index, int role) const{
	if(!index.isValid() || role != Qt::DisplayRole)
		return QVariant();

	const Query& query = _queries[index.row()];

	if(index.column() == 0){
		switch(query.status){
			case Satisfied:
			case NotSatisfiable:
			case Inprogress:
			case Unknown:
			default:
				return QIcon::fromTheme("gnome-unknown");
			//TODO: Find icons here: http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
			// or include some in resources and use them here...
		}
	}

	if(index.column() == 1)
		return query.name;

	if(index.column() == 2)
		return "50%";	//TODO: Do something better than this :)

	return QVariant();
}

QVariant QueryModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if(role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();
	if(section == 0)
		return "";
	else if(section == 1)
		return tr("Description");
	else if(section == 2)
		return tr("Status");
	return QVariant();
}


/** Add a new query (opens a dialog) */
void QueryModel::addQuery(QWidget *parent){
	Query query;
	query.name = "New query";
	query.query = "";
	query.status = Unknown;
	query.strategy = "";
	QueryDialog d(query, parent);
	d.setIdentifiers(_net->placeNames(), _net->variableNames());
	if(d.exec() == QDialog::Accepted){
		query.name = d.name();
		query.query = d.query();
		query.strategy = d.strategy();
		AddRemoveQueryCommand* cmd = new AddRemoveQueryCommand(this, query, true);
		_net->undoStack()->push(cmd);
	}
}

/** Edit query */
void QueryModel::editQuery(const QModelIndex& index, QWidget *parent){
	if(!index.isValid()) return;
	Q_ASSERT(index.row() < rowCount());

	QueryDialog d(_queries[index.row()], parent);
	d.setIdentifiers(_net->placeNames(), _net->variableNames());
	if(d.exec() == QDialog::Accepted){
		Query q = _queries[index.row()];
		q.name = d.name();
		q.query = d.query();
		q.strategy = d.strategy();
		if(q == _queries[index.row()])
			return; //There's no changes
		EditQueryCommand* cmd = new EditQueryCommand(this, index.row(), q);
		_net->undoStack()->push(cmd);
	}
}

/** Remove query */
void QueryModel::removeQuery(const QModelIndex& index){
	if(!index.isValid()) return;
	Q_ASSERT(index.row() < rowCount());
	AddRemoveQueryCommand* cmd = new AddRemoveQueryCommand(this, _queries[index.row()], false);
	_net->undoStack()->push(cmd);
}

/** Run all queries */
void QueryModel::runAll(){}

/** Run a specific query */
void QueryModel::runQuery(const QModelIndex& index){}

/** Break execution of all running queries */
void QueryModel::stopAll(){}

/** Break execution of a specific query */
void QueryModel::stopQuery(const QModelIndex& index){}
