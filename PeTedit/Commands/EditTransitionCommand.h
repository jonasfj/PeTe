/* PeTe - Petri Engine exTremE
 * Copyright (C) 2011  Jonas Finnemann Jensen <jopsen@gmail.com>,
 *                     Thomas Søndersø Nielsen <primogens@gmail.com>,
 *                     Lars Kærlund Østergaard <larsko@gmail.com>,
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
