#include "NetItem.h"
#include "ArcItem.h"

void NetItem::updateConnectedItems(){
	foreach(ArcItem* item, this->_connectedItems)
		item->updateEndPoints();
}
