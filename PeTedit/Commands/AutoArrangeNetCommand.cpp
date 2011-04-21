#include "AutoArrangeNetCommand.h"

#include "../NetItems/NetItem.h"

#include <PetriEngine/LayoutBuilder.h>

AutoArrangeNetCommand::AutoArrangeNetCommand(PetriNetScene* scene){
	_scene = scene;
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

void AutoArrangeNetCommand::redo(){
	//apply after list
	foreach(const NetItemPositionListBuilder::NetItemPosition& itemPos, afterList)
		itemPos.item->setPos(itemPos.x, itemPos.y);
	_scene->updateSceneRect();
}

void AutoArrangeNetCommand::undo(){
	//apply before list
	foreach(const NetItemPositionListBuilder::NetItemPosition& itemPos, beforeList)
		itemPos.item->setPos(itemPos.x, itemPos.y);
	_scene->updateSceneRect();
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
