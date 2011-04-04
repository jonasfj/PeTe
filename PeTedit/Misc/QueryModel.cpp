#include "QueryModel.h"

#include "../NetItems/PetriNetScene.h"
#include "../Commands/AddRemoveQueryCommand.h"
#include "../Commands/EditQueryCommand.h"
#include "../Dialogs/QueryDialog.h"
#include "QueryThread.h"

#include <QIcon>

QueryModel::QueryModel(PetriNetScene* net)
	 : QAbstractTableModel(net){
	_net = net;
}

QueryModel::~QueryModel(){
	//TODO: Stop and delete all threads... :)
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

	//Return double if you want progress bar, text if you wan't text
	if(index.column() == 2)
		return 0.5;	//TODO: Do something better than this :)

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

/******************** Query Editing Slots ********************/

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
		AddRemoveQueryCommand* cmd = new AddRemoveQueryCommand(this, query);
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
	AddRemoveQueryCommand* cmd = new AddRemoveQueryCommand(this, _queries[index.row()]);
	_net->undoStack()->push(cmd);
}

/******************** Query Editing Methods ********************/

/** Insert query (no undo command will be created!)
 * @param row	Row to insert at, -1 if end of list.
 * @returns row number that was created...
 */
int QueryModel::insertQuery(const Query& query, int row){
	if(row == -1)
		row = _queries.size();
	//Alert views that we're inserting a row
	this->beginInsertRows(QModelIndex(), row, row);
	_threads.insert(row, NULL);
	_queries.insert(row, query);
	this->endInsertRows();
	return row;
}

/** Remove query (no undo command will be created!) */
QueryModel::Query QueryModel::takeQuery(int row){
	Q_ASSERT(0 <= row && row < _queries.size());
	this->beginRemoveRows(QModelIndex(), row, row);
	abortThread(row);
	_threads.removeAt(row);
	Query q = _queries.takeAt(row);
	this->endRemoveRows();
	return q;
}

/** Get a query */
const QueryModel::Query& QueryModel::query(int row){
	Q_ASSERT(0 <= row && row < _queries.size());
	return _queries.at(row);
}

/** Set a query (no undo command will be created!) */
void QueryModel::setQuery(const Query& query, int row){
	abortThread(row);
	_queries[row] = query;
	emit dataChanged(this->index(row, 0), this->index(row,2));
}

/******************** Query Exection Slots ********************/

/** Run all queries */
void QueryModel::runAll(){
	for(size_t i = 0; i < _queries.size(); i++)
		startThread(i);
}

/** Run a specific query */
void QueryModel::runQuery(const QModelIndex& index){
	if(!index.isValid()) return;
	startThread(index.row());
}

/** Break execution of all running queries */
void QueryModel::stopAll(){
	for(size_t i = 0; i < _queries.size(); i++)
		abortThread(i);
}

/** Break execution of a specific query */
void QueryModel::stopQuery(const QModelIndex& index){
	if(!index.isValid()) return;
	abortThread(index.row());
}

/******************** Query Exection Methods ********************/

void QueryModel::abortThread(int row){
	//TODO: Update icon (status)
	Q_ASSERT(0 <= row && row < _queries.size());
	if(_threads[row]){
		disconnect(_threads[row], SIGNAL(completed(QueryThread*)), this, SLOT(completedThread(QueryThread*)));
		connect(_threads[row], SIGNAL(finished()), _threads[row], SLOT(deleteLater()));
		connect(_threads[row], SIGNAL(terminated()), _threads[row], SLOT(deleteLater()));
		_threads[row]->abort();
	}
	_threads[row] = NULL;
}

void QueryModel::startThread(int row){
	//TODO: Update icon (status)
	Q_ASSERT(0 <= row && row < _queries.size());
	abortThread(row);
	_threads[row] = new QueryThread(_queries[row].query, _queries[row].strategy, _net, NULL);
	//TODO: Connect to progressChanged signal
	connect(_threads[row], SIGNAL(completed(QueryThread*)), this, SLOT(completedThread(QueryThread*)));
	_threads[row]->start();
}

void QueryModel::completedThread(QueryThread *thread){
	Q_ASSERT(thread != NULL);
	thread->deleteLater();
	int row = _threads.indexOf(thread);
	if(row < 0)
		return;

	//TODO: Update icon (status)

	_threads[row] = NULL;
	if(thread->result().result() == PetriEngine::Reachability::ReachabilityResult::Satisfied)
		_queries[row].status = Satisfied;
	else if(thread->result().result() == PetriEngine::Reachability::ReachabilityResult::NotSatisfied)
		_queries[row].status = NotSatisfiable;
	else
		_queries[row].status = Unknown;
}
