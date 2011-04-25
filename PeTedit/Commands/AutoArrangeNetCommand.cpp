#include "AutoArrangeNetCommand.h"

#include "../NetItems/NetItem.h"

#include <PetriEngine/LayoutBuilder.h>
#include <float.h>

#define	 MIN(a,b)	(a < b ? a : b)
#define	 MAX(a,b)	(a > b ? a : b)

AutoArrangeNetCommand::AutoArrangeNetCommand(PetriNetScene* scene){
	_scene = scene;
	setText(QObject::tr("auto arrange"));
	// Create list with position before auto arrange
	NetItemPositionListBuilder beforeListBuilder(scene);
	scene->produce(&beforeListBuilder);
	beforeList = beforeListBuilder.makeNetItemPositionList();
	// Generate fancy layout
	PetriEngine::LayoutBuilder layout;
	scene->produce(&layout);
	NetItemPositionListBuilder afterListBuilder(scene);
	layout.produce(&afterListBuilder);
	afterList = afterListBuilder.makeNetItemPositionList();
}

AutoArrangeNetCommand::AutoArrangeNetCommand(PetriNetScene* scene,
											 QList<NetItem*> items,
											 Qt::Orientation alignOn){
	if(alignOn == Qt::Horizontal)
		setText(QObject::tr("align horizonal"));
	else
		setText(QObject::tr("align vertical"));
	_scene = scene;
	double minx = DBL_MAX, miny = DBL_MAX, maxx = -DBL_MAX, maxy = -DBL_MAX;
	foreach(NetItem* item, items){
		// Find min and max
		minx = MIN(minx, item->pos().x());
		miny = MIN(miny, item->pos().y());
		maxx = MAX(maxx, item->pos().x());
		maxy = MAX(maxy, item->pos().y());
		// Build before list
		NetItemPositionListBuilder::NetItemPosition itemPos;
		itemPos.item = item;
		itemPos.x = item->pos().x();
		itemPos.y = item->pos().y();
		beforeList.append(itemPos);
	}
	double cx = minx + (maxx - minx) / 2;
	double cy = miny + (maxy - miny) / 2;
	foreach(NetItem* item, items){
		// Build after list
		NetItemPositionListBuilder::NetItemPosition itemPos;
		itemPos.item = item;
		itemPos.x = alignOn == Qt::Horizontal ? cx : item->pos().x();
		itemPos.y = alignOn == Qt::Vertical ? cy : item->pos().y();
		afterList.append(itemPos);
	}
}

void AutoArrangeNetCommand::redo(){
	//apply after list
	foreach(const NetItemPositionListBuilder::NetItemPosition& itemPos, afterList)
		itemPos.item->setPos(itemPos.x, itemPos.y);
	_scene->updateSceneRect();
	_scene->update();
}

void AutoArrangeNetCommand::undo(){
	//apply before list
	foreach(const NetItemPositionListBuilder::NetItemPosition& itemPos, beforeList)
		itemPos.item->setPos(itemPos.x, itemPos.y);
	_scene->updateSceneRect();
	_scene->update();
}

void AutoArrangeNetCommand
	 ::NetItemPositionListBuilder
	 ::addVariable(const std::string &, int, int){}

void AutoArrangeNetCommand
	 ::NetItemPositionListBuilder
	 ::addPlace(const std::string &name, int, double x, double y){
	NetItemPosition itemPos;
	itemPos.item = scene->findNetItem(name.c_str());
	itemPos.x = x;
	itemPos.y = y;
	Q_ASSERT(itemPos.item);
	if(itemPos.item)
		list.append(itemPos);
}

void AutoArrangeNetCommand
	 ::NetItemPositionListBuilder
	 ::addTransition(const std::string &name, const std::string&, const std::string&, double x, double y){
	NetItemPosition itemPos;
	itemPos.item = scene->findNetItem(name.c_str());
	itemPos.x = x;
	itemPos.y = y;
	Q_ASSERT(itemPos.item);
	if(itemPos.item)
		list.append(itemPos);
}

void AutoArrangeNetCommand
	 ::NetItemPositionListBuilder
	 ::addInputArc(const std::string&, const std::string&, int){}

void AutoArrangeNetCommand
	 ::NetItemPositionListBuilder
	 ::addOutputArc(const std::string&, const std::string&, int){}
