#ifndef RENAMEITEMCOMMAND_H
#define RENAMEITEMCOMMAND_H

#include <QUndoCommand>
#include <QtGlobal>

class NetItem;

class RenameItemCommand : public QUndoCommand {
public:
	RenameItemCommand(NetItem* item, const QString& name);
	void undo();
	void redo();
private:
	void swap();
	QString _newname;
	NetItem* _item;
};

#endif // RENAMEITEMCOMMAND_H
