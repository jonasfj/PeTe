#ifndef QUERYMODEL_H
#define QUERYMODEL_H

#include <QAbstractTableModel>

class PetriNetScene;
class AddRemoveQueryCommand;
class EditQueryCommand;

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

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section,
						Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;

private:
	QList<Query> _queries;
	PetriNetScene* _net;
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

	friend class AddRemoveQueryCommand;
	friend class EditQueryCommand;
};

#endif // QUERYMODEL_H
