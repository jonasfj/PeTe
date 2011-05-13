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
#ifndef VALIDATIONISSUESMODEL_H
#define VALIDATIONISSUESMODEL_H

#include <QAbstractTableModel>

#include <PetriEngine/ValidationBuilder.h>

#include <vector>

class PetriNetScene;

/** Model for presenting validation issues */
class ValidationIssuesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ValidationIssuesModel(QObject *parent = 0);
	/** Set issues */
	void setIssues(const std::vector<PetriEngine::ValidationError>& issues);

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section,
						Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	void showValidationIssue(PetriNetScene* scene, const QModelIndex &index);
private:
	std::vector<PetriEngine::ValidationError> _issues;
signals:

public slots:
	/** Clear all issues */
	void clear();
};

#endif // VALIDATIONISSUESMODEL_H
