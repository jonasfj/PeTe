#ifndef QUERYMODEL_H
#define QUERYMODEL_H

#include <QAbstractTableModel>
#include <QIcon>

#include "PetriEngine/Reachability/ReachabilityResult.h"

class PetriNetScene;
class QueryThread;

/** Model for presenting the list of queries */
class QueryModel : public QAbstractTableModel
{
    Q_OBJECT

	/** State that a query can be in */
	struct QueryState{
		QueryState(){
			thread = NULL;
			progress = 0;
			time = 0;
		}
		QueryThread* thread;
		double progress;
		double time;
		PetriEngine::Reachability::ReachabilityResult result;
	};
public:
	/** Internal representation of a query */
	struct Query{
		QString name,
				query,
				strategy;
		bool jit;
		bool operator==(const Query &other){
			return name == other.name &&
				   query == other.query &&
				   strategy == other.strategy &&
				   jit == other.jit;
		}
	};

	explicit QueryModel(PetriNetScene* net);

	~QueryModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section,
						Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;

	/** Insert query (no undo command will be created!)
	 * @param row	Row to insert at, -1 if end of list.
	 * @returns row number that was created...
	 */
	int insertQuery(const Query& query, int row = -1);
	/** Remove query (no undo command will be created!) */
	Query takeQuery(int row);

	/** Get a query */
	const Query& query(int row);
	/** Set a query (no undo command will be created!) */
	void setQuery(const Query& query, int row);

private:
	QList<QueryState> _qstate;
	QList<Query> _queries;
	PetriNetScene* _net;

	/** Signal that a row was changed */
	void emitDataChanged(int row);

	/** Abort the thread for a row, if any */
	void abortThread(int row);
	/** Start a new thread, abort old if there's one */
	void startThread(int row);

	QIcon _clockIcon;
	QIcon _checkIcon;
	QIcon _crossIcon;
	QIcon _unknownIcon;

private slots:
	void completedThread(QueryThread* thread, qreal time);
	void progressReported(QueryThread* thread, qreal progress, qreal time);
signals:

public slots:
	/** Add a new query (opens a dialog) */
	void addQuery(QWidget* parent);
	/** Edit query (opens a dialog) */
	void editQuery(const QModelIndex& index, QWidget* parent);
	/** Remove query */
	void removeQuery(const QModelIndex& index);
	/** Run a specific query */
	void runQuery(const QModelIndex& index);
	/** Break execution of a specific query */
	void stopQuery(const QModelIndex& index);
};

#endif // QUERYMODEL_H
