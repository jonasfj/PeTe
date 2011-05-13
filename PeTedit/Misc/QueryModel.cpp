/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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

	_clockIcon = QIcon(":/Icons/24x24/clock.png");
	_checkIcon = QIcon(":/Icons/24x24/check.png");
	_crossIcon = QIcon(":/Icons/24x24/cross.png");
	_unknownIcon = QIcon(":/Icons/24x24/unknown.png");
}

QueryModel::~QueryModel(){
	for(int i = 0; i < _qstate.length(); i++)
		abortThread(i);
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
			return tr("Satisfiability unknown, run query to find it");
		return state.result.explanation().c_str();
	}

	//Display stats in status bar
	if(role == Qt::StatusTipRole){
		QString retval  = "";
		if(!state.result.explanation().empty()){
			retval += tr("expanded states: ") + QString::number(state.result.expandedStates()) + " ";
			retval += tr("explored states: ") + QString::number(state.result.exploredStates());
		}
		return retval;
	}

	if(role == DataRoles::ProgressText &&
		index.column() == COL_PROGRESS &&
		state.thread){
		return tr("elapsed %1").arg(state.time, 3);
	}

	if(index.column() == COL_DESCRIPTION && role == Qt::DecorationRole){
		if(state.thread)
			return _clockIcon;

		switch(state.result.result()){
			case PetriEngine::Reachability::ReachabilityResult::Satisfied:
				return _checkIcon;
			case PetriEngine::Reachability::ReachabilityResult::NotSatisfied:
				return _crossIcon;
			case PetriEngine::Reachability::ReachabilityResult::Unknown:
			default:
				return _unknownIcon;
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
		return tr("finished in %1s").arg(state.time);
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
	Query q;
	q.name = "New query";
	q.query = "";
	q.strategy = "";
	q.jit = false;
	QueryDialog d(q, parent);
	d.setIdentifiers(_net->placeNames(), _net->variableNames());
	if(d.exec() == QDialog::Accepted){
		q.name = d.name();
		q.query = d.query();
		q.strategy = d.strategy();
		q.jit = d.jit();
		AddRemoveQueryCommand* cmd = new AddRemoveQueryCommand(this, q);
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
		q.jit = d.jit();
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
	AddRemoveQueryCommand* cmd = new AddRemoveQueryCommand(this, index.row());
	_net->undoStack()->push(cmd);
}

void QueryModel::importSUMoQueries(QIODevice &f){
	if(!f.isOpen())
		f.open(QIODevice::ReadOnly);
	QUndoCommand* cmdStack = new QUndoCommand();
	while(!f.atEnd()){
		QByteArray line = f.readLine();
		if(line.size() == 0)
			continue;
		PetriEngine::PQL::SUMoQuery c = PetriEngine::PQL::ParseSUMoQuery(line.data());
		if(!c.query || c.isInvariant)
			continue;
		Query q;
		//You can hardcode scaling into the queries here
		//This will only work for queries where all literals need be scaled
		//c.query->scale(320);	//Yes, just hardcode for the imports we need...
		//Please do remember to comment it out again, or I will!!!
		// - write you a polite letter explaining the situation :)
		q.name = c.name.c_str();
		q.query = c.query->toString().c_str();
		q.strategy = "";
		q.jit = false;
		new AddRemoveQueryCommand(this, q, cmdStack);
	}
	if(cmdStack->childCount() == 0)
		delete cmdStack;
	else
		_net->undoStack()->push(cmdStack);
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
	_qstate[row].thread = new QueryThread(_queries[row].query,
										  _queries[row].strategy,
										  _net,
										  _queries[row].jit,
										  NULL);

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
	_qstate[row].time = time == 0 ? 0.01 : time;
	_qstate[row].result = thread->result();
	_qstate[row].thread = NULL;
	emitDataChanged(row);
}
