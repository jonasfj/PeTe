#include "InsertArcCommand.h"

#include "../NetItems/ArcItem.h"
#include "../NetItems/PetriNetScene.h"

InsertArcCommand::InsertArcCommand(PetriNetScene* scene, ArcItem* arc){
	_scene = scene;
	_arc = arc;
	_arc->unregisterAtEndPoints();
	_removed = true;
	this->setText("Insert arc");
}

InsertArcCommand::~InsertArcCommand(){
	if(_removed && _arc)
		delete _arc;
	_arc = NULL;
}

void InsertArcCommand::undo(){
	_arc->unregisterAtEndPoints();
	_scene->removeArc(_arc);
	_removed = true;
}

void InsertArcCommand::redo(){
	_scene->addArc(_arc);
	_arc->registerAtEndPoints();
	_removed = false;
	_arc->updateEndPoints();
}

