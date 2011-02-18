#include "inserttransitioncommand.h"

#include "../netitems/transitionitem.h"
#include "../netitems/petrinetscene.h"

InsertTransitionCommand::InsertTransitionCommand(PetriNetScene* scene, QPointF position){
	_removed = true;
	_scene = scene;
	int number = 0;
	QString name;
	do{
		number += 1;
		name =  "T " + QString::number(number);
	}while(_scene->findTransition(name));
	_transition = new TransitionItem(position, name);
	this->setText("Created \"" + name + "\"");
}

InsertTransitionCommand::~InsertTransitionCommand(){
	if(_removed)
		delete _transition;
	_transition = NULL;
}

void InsertTransitionCommand::undo(){
	Q_ASSERT(_removed == false);
	_scene->removeTransition(_transition);
	_removed = true;
}

void InsertTransitionCommand::redo(){
	Q_ASSERT(_removed == true);
	_scene->addTransition(_transition);
	_removed = false;
}
