#include "QueryListBuilder.h"

void QueryListBuilder::addQuery(const QueryModel::Query &query){
	queries.push_back(query);
}
