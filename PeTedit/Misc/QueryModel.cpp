#include "QueryModel.h"

#include "../NetItems/PetriNetScene.h"
#include "../Commands/AddRemoveQueryCommand.h"
#include "../Commands/EditQueryCommand.h"
#include "../Dialogs/QueryDialog.h"
#include "QueryThread.h"
#include "ValidationIssuesModel.h"
#include "CustomDataRoles.h"

#include <QIcon>

/** Description column number */
#define COL_DESCRIPTION		0

/** Progress column number */
#define COL_PROGRESS		1

/** Number of colums */
#define COL_COUNT			2


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
	return COL_COUNT;	// Name, Progress
}

QVariant QueryModel::data(const QModelIndex &index, int role) const{
	if(!index.isValid())
		return QVariant();

	const Query& query = _queries[index.row()];
	const QueryState& state = _qstate[index.row()];

	if(role == Qt::ToolTipRole){
		if(state.result.explanation().empty())
			return "Satisfiability unknown, run query to find it";
		return state.result.explanation().c_str();
	}

	if(role == DataRoles::ProgressText &&
		index.column() == COL_PROGRESS &&
		state.thread){
		return tr("elapsed %1").arg(state.time);
	}

	if(index.column() == COL_DESCRIPTION && role == Qt::DecorationRole){
		if(state.thread)
			return QIcon(":/Icons/clock.svg");

		switch(state.result.result()){
			case PetriEngine::Reachability::ReachabilityResult::Satisfied:
				return QIcon(":/Icons/check.svg");
			case PetriEngine::Reachability::ReachabilityResult::NotSatisfied:
				return QIcon(":/Icons/cross.svg");
			case PetriEngine::Reachability::ReachabilityResult::Unknown:
			default:
				return QIcon(":/Icons/unknown.svg");
		}
	}

	if(role != Qt::DisplayRole)
		return QVariant();

	if(index.column() == COL_DESCRIPTION)
		return query.name;

	//Return double if you want progress bar, text if you wan't text
	if(index.column() == COL_PROGRESS){
		if(state.thread)
			return state.progress;
		if(state.time == 0)
			return "-";
		return tr("finished in %1").arg(state.time);
	}

	return QVariant();
}

QVariant QueryModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if(role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();
	if(section == COL_DESCRIPTION)
		return tr("Description");
	else if(section == COL_PROGRESS)
		return tr("Status");
	return QVariant();
}

void QueryModel::emitDataChanged(int row){
	emit dataChanged(this->index(row, 0), this->index(row, COL_COUNT));
}

/******************** Query Editing Slots ********************/

/** Add a new query (opens a dialog) */
void QueryModel::addQuery(QWidget *parent){
	Query query;
	query.name = "New query";
	query.query = "";
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
	_qstate.insert(row, QueryState());
	_queries.insert(row, query);
	this->endInsertRows();
	return row;
}

/** Remove query (no undo command will be created!) */
QueryModel::Query QueryModel::takeQuery(int row){
	Q_ASSERT(0 <= row && row < _queries.size());
	this->beginRemoveRows(QModelIndex(), row, row);
	abortThread(row);
	_qstate.removeAt(row);
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
	emitDataChanged(row);
}

/******************** Query Exection Slots ********************/

/** Run a specific query */
void QueryModel::runQuery(const QModelIndex& index){
	if(!index.isValid()) return;
	_net->validate();
	if(_net->validationIssues()->rowCount() != 0)
	return;
	startThread(index.row());
}

/** Break execution of a specific query */
void QueryModel::stopQuery(const QModelIndex& index){
	if(!index.isValid()) return;
	abortThread(index.row());
}

/******************** Query Exection Methods ********************/

void QueryModel::abortThread(int row){
	Q_ASSERT(0 <= row && row < _queries.size());
	QueryThread*& thread = _qstate[row].thread;
	if(thread){
		disconnect(thread, SIGNAL(completed(QueryThread*,qreal)),
				   this, SLOT(completedThread(QueryThread*,qreal)));
		disconnect(thread, SIGNAL(progressChanged(QueryThread*,qreal,qreal)),
				  this, SLOT(progressReported(QueryThread*,qreal,qreal)));
		connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
		connect(thread, SIGNAL(terminated()), thread, SLOT(deleteLater()));
		thread->abort();
	}
	//Clear the state
	_qstate[row] = QueryState();
	emitDataChanged(row);
}

void QueryModel::startThread(int row){
	Q_ASSERT(0 <= row && row < _queries.size());
	//Abort existing thread and/or clear state
	abortThread(row);
	_qstate[row].thread = new QueryThread(_queries[row].query, _queries[row].strategy, _net, NULL);

	connect(_qstate[row].thread, SIGNAL(progressChanged(QueryThread*,qreal,qreal)),
			this, SLOT(progressReported(QueryThread*,qreal,qreal)));
	connect(_qstate[row].thread, SIGNAL(completed(QueryThread*,qreal)),
			this, SLOT(completedThread(QueryThread*,qreal)));
	_qstate[row].thread->start();
	emitDataChanged(row);
}

void QueryModel::progressReported(QueryThread *thread, qreal progress, qreal time){
	Q_ASSERT(thread != NULL);
	int row = -1;
	for(int i = 0; i < _qstate.size(); i++){
		if(_qstate[i].thread == thread){
			row = i;
			break;
		}
	}
	if(row == -1){
		thread->abort();
		disconnect(thread, SIGNAL(progressChanged(QueryThread*,qreal,qreal)),
				  this, SLOT(progressReported(QueryThread*,qreal,qreal)));
		connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
		connect(thread, SIGNAL(terminated()), thread, SLOT(deleteLater()));
		return;
	}

	_qstate[row].progress = progress;
	_qstate[row].time = time;
	emitDataChanged(row);
}

void QueryModel::completedThread(QueryThread *thread, qreal time){
	Q_ASSERT(thread != NULL);
	thread->deleteLater();
	int row = -1;
	for(int i = 0; i < _qstate.size(); i++){
		if(_qstate[i].thread == thread){
			row = i;
			break;
		}
	}
	if(row == -1)
		return;

	_qstate[row].progress = 0;
	_qstate[row].time = time;
	_qstate[row].result = thread->result();
	_qstate[row].thread = NULL;
	emitDataChanged(row);
}
