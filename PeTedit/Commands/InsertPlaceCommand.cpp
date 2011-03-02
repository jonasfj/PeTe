#include "InsertPlaceCommand.h"

#include "../NetItems/PlaceItem.h"
#include "../NetItems/PetriNetScene.h"

#include <QtGlobal>

InsertPlaceCommand::InsertPlaceCommand(PetriNetScene *scene, QPointF position){
	_scene = scene;
	_removed = true;
	int number = 0;
	QString name;
	do{
		number += 1;
		name =  "P" + QString::number(number);
	}while(_scene->findNetItem(name));
	_place = new PlaceItem(position, name);
	this->setText("Created \"" + name + "\"");
}

InsertPlaceCommand::~InsertPlaceCommand(){
	if(_removed)
		delete _place;
	_place = NULL;
}

void InsertPlaceCommand::undo(){
	Q_ASSERT(_removed == false);
	_scene->removeNetItem(_place);
	_removed = true;
}

void InsertPlaceCommand::redo(){
	Q_ASSERT(_removed == true);
	_scene->addNetItem(_place);
	_removed = false;
}
