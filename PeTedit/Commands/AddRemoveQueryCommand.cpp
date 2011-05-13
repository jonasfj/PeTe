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
#include "AddRemoveQueryCommand.h"

#include "../Misc/QueryModel.h"

AddRemoveQueryCommand::AddRemoveQueryCommand(QueryModel* model,
											 const QueryModel::Query& query,
											 QUndoCommand* parent)
	: QUndoCommand(parent){
	_model = model;
	_query = query;
	_add = true;
	_row = -1;
}

AddRemoveQueryCommand::AddRemoveQueryCommand(QueryModel* model,
											 int row){
	_model = model;
	_add = false;
	_row = row;
}

AddRemoveQueryCommand::~AddRemoveQueryCommand(){}

void AddRemoveQueryCommand::redo(){
	swap();
}

void AddRemoveQueryCommand::undo(){
	swap();
}

void AddRemoveQueryCommand::swap(){
	if(_add)
		_row = _model->insertQuery(_query, _row);
	else{
		Q_ASSERT(_row != -1);
		_query = _model->takeQuery(_row);
	}
	_add = !_add;
}
