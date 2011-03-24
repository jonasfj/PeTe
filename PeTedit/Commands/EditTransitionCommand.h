#ifndef EDITTRANSITIONCOMMAND_H
#define EDITTRANSITIONCOMMAND_H

#include <QUndoCommand>
#include <QtGlobal>
#include "RenameItemCommand.h"

class TransitionItem;

class EditTransitionCommand : public RenameItemCommand
{
public:
	EditTransitionCommand(TransitionItem* item, const QString& name,
						  const QString& preconditions,
						  const QString& postconditions);

	void undo();
	void redo();
protected:
	QString _newPreConditions;
	QString _newPostConditions;
private:
	void swap();
};

#endif // EDITTRANSITIONCOMMAND_H
