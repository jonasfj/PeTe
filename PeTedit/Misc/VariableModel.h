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
#ifndef VARIABLEMODEL_H
#define VARIABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
class PetriNetScene;

/** Model for representing variables in a PNDV */
class VariableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	/** A variable structure */
	struct Variable{
		Variable(){
			name = "";
			value=0;
			range=0;
		}
		QString name;
		int value;
		int range;
	};

	explicit VariableModel(PetriNetScene* net);

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section,
						Qt::Orientation orientation,
						int role) const;

	bool setData(const QModelIndex &index, const QVariant &value, int role);
	void emitDataChanged(int row);
	Qt::ItemFlags flags(const QModelIndex &index) const;

	void addVariable(QString name="", int value=0, int range=0);
	void removeVariable(const QModelIndex& index);

	const Variable& variable(int row) const{
		return _variables.at(row);
	}

	QStringList variableNames() const{
		QStringList names;
		foreach(const Variable& var, _variables){
			names.append(var.name);
		}
		return names;
	}

private:
	QList<Variable> _variables;
	PetriNetScene* _net;
};

#endif // VARIABLEMODEL_H
