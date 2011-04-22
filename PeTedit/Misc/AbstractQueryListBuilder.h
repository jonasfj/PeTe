#ifndef ABSTRACTQUERYLISTBUILDER_H
#define ABSTRACTQUERYLISTBUILDER_H

#include <string>
#include "QueryModel.h"

class AbstractQueryListBuilder {
public:
	/** Add a new query */
	virtual void addQuery(const QueryModel::Query& query) = 0;
};

#endif // ABSTRACTQUERYLISTBUILDER_H
