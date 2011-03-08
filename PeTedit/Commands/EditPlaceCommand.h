#ifndef EDITPLACECOMMAND_H
#define EDITPLACECOMMAND_H

#include "RenameItemCommand.h"
#include <QtGlobal>

class PlaceItem;

class EditPlaceCommand : public RenameItemCommand {
public:
	EditPlaceCommand(PlaceItem* place, const QString& name, int tokens);
	void undo();
	void redo();
private:
	void swap();
	int _tokens;
};

#endif // EDITPLACECOMMAND_H
