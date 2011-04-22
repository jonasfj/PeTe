#ifndef QUERYLISTBUILDER_H
#define QUERYLISTBUILDER_H

#include "AbstractQueryListBuilder.h"
#include "QueryModel.h"
#include <QList>

class QueryListBuilder : public AbstractQueryListBuilder {
public:
	void addQuery(const QueryModel::Query& query);
	QList<QueryModel::Query> makeQueries(){return queries;}
private:
	QList<QueryModel::Query> queries;
};

#endif // QUERYLISTBUILDER_H
