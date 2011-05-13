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
#include "VariableModel.h"
#include "../NetItems/PetriNetScene.h"

#define COL_NAME	0
#define COL_VALUE	1
#define COL_RANGE	2
#define COL_COUNT	3

VariableModel::VariableModel(PetriNetScene* net)
	: QAbstractTableModel(net)
{
	_net = net;
}

int VariableModel::columnCount(const QModelIndex &parent) const{
	if(parent.isValid()) return 0;
	return COL_COUNT;
}

int VariableModel::rowCount(const QModelIndex &parent) const{
	if(parent.isValid()) return 0;
	return _variables.count();
}

QVariant VariableModel::data(const QModelIndex &index, int role) const{
	if(!index.isValid() || (role != Qt::EditRole && role != Qt::DisplayRole))
		return QVariant();

	const Variable& variable = _variables.at(index.row());

	if(index.column() == COL_NAME)
		return variable.name;
	if(index.column() == COL_VALUE)
		return variable.value;
	if(index.column() == COL_RANGE)
		return variable.range;

	return QVariant();
}

bool VariableModel::setData(const QModelIndex &index, const QVariant &value, int role){
	if(!index.isValid() || role != Qt::EditRole)
		return false;
	bool retVal = false;
	Variable& variable = _variables[index.row()];

	if(index.column() == COL_NAME){
		if(_net->isValidAvailableIdentifier(value.toString()) || variable.name == value.toString()){
			variable.name = value.toString();
			retVal = true;
		}
	}
	if(index.column() == COL_VALUE){
		int newValue = value.toInt();
		if(newValue <= variable.range && newValue >= 0){
			variable.value = newValue;
			retVal = true;
		} else if(newValue >= 0){
			variable.value = variable.range;
		}
	}
	if(index.column() == COL_RANGE){
		int newValue = value.toInt();
		if(newValue < 0){
			variable.range = 0;
			variable.value = 0;
		}
		if(newValue <= variable.value){
			variable.value = newValue;
			variable.range = newValue;
		}else{
			variable.range = newValue;
			retVal = true;
		}
	}
	emitDataChanged(index.row());
	return retVal;
}

QVariant VariableModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if(role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();
	if(section == COL_NAME)
		return tr("Name");
	else if(section == COL_VALUE)
		return tr("Value");
	else if(section == COL_RANGE)
		return tr("Range");
	return QVariant();
}

Qt::ItemFlags VariableModel::flags(const QModelIndex &) const{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void VariableModel::emitDataChanged(int row){
	emit dataChanged(this->index(row, 0), this->index(row, COL_COUNT));
}

/** Adds a new variable to the model */
void VariableModel::addVariable(QString name, int value, int range){
	int row = _variables.size();
	this->beginInsertRows(QModelIndex(), row, row);

	int num=0;
	while(!_net->isValidAvailableIdentifier(name)) {
		name = "x" + QString::number(num++);
	}

	Variable v;
	v.name = name;
	v.value=value;
	v.range=range;
	_variables.insert(row,v);
	this->endInsertRows();
}

/** Deletes a variable from the model */
void VariableModel::removeVariable(const QModelIndex &index){
	this->beginRemoveRows(QModelIndex(),index.row(),index.row());
	_variables.removeAt(index.row());
	this->endRemoveRows();
}
