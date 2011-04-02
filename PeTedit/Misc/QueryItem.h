#ifndef QUERYITEM_H
#define QUERYITEM_H

#include <QStandardItem>

/** Item for a QStandardItemModel for queries */
class QueryItem : public QStandardItem
{
public:
	/** Different types of query items */
	enum QueryTypes{
		ReachabilityQuery = QStandardItem::UserType,
		UnknownQuery
	};

	QueryItem();

	/** Get data*/
	QVariant data(int role) const;

	/** Returns a value from QueryTypes */
	int type() const;

	const QString& name() const{return _name;}
	const QString& query() const{return _query;}
	const QString& strategy() const{return _strategy;}

	void setName(const QString& name) {
		_name = name;
		emitDataChanged();
	}
	void setQuery(const QString& query) {_query = query;}
	void setStrategy(const QString& strategy) {_strategy = strategy;}
private:
	QString _name;
	QString _query;
	QString _strategy;
};

#endif // QUERYITEM_H
