#ifndef QUERYMODEL_H
#define QUERYMODEL_H

#include <QAbstractTableModel>

class PetriNetScene;
class QueryThread;

/** Model for presenting the list of queries */
class QueryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
	/** Different statuses that a query can have */
	enum QueryStatus{
		Satisfied,
		NotSatisfiable,
		Inprogress,
		Unknown
	};
	/** Internal representation of a query */
	struct Query{
		QString name,
				query,
				strategy;
		QueryStatus status;
		bool operator==(const Query &other){
			return name == other.name &&
				   query == other.query &&
				   strategy == other.strategy;
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
	QList<QueryThread*> _threads;
	QList<Query> _queries;
	PetriNetScene* _net;

	/** Abort the thread for a row, if any */
	void abortThread(int row);
	/** Start a new thread, abort old if there's one */
	void startThread(int row);
private slots:
	void completedThread(QueryThread* thread);
signals:

public slots:
	/** Add a new query (opens a dialog) */
	void addQuery(QWidget* parent);
	/** Edit query (opens a dialog) */
	void editQuery(const QModelIndex& index, QWidget* parent);
	/** Remove query */
	void removeQuery(const QModelIndex& index);
	/** Run all queries */
	void runAll();
	/** Run a specific query */
	void runQuery(const QModelIndex& index);
	/** Break execution of all running queries */
	void stopAll();
	/** Break execution of a specific query */
	void stopQuery(const QModelIndex& index);
};

#endif // QUERYMODEL_H
