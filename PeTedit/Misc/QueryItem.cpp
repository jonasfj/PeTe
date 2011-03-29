#include "QueryItem.h"

QueryItem::QueryItem() : QStandardItem(){
	_name = "New query";
}

int QueryItem::type() const{
	return ReachabilityQuery;
}

QVariant QueryItem::data(int role) const{
	if(role == Qt::DisplayRole || role == Qt::EditRole){
		return name();
	}
	//TODO: Support tooltip role etc...
	return QVariant();
}
