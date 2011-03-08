#ifndef EDITARCCOMMAND_H
#define EDITARCCOMMAND_H

#include <QUndoCommand>
#include <QtGlobal>

class ArcItem;

class EditArcCommand : public QUndoCommand {
public:
	EditArcCommand(ArcItem* arc, int weight);
	void undo();
	void redo();
private:
	void swap();
	ArcItem* _arc;
	int _newWeight;
};

#endif // EDITARCCOMMAND_H
