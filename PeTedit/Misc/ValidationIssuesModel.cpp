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
#include "ValidationIssuesModel.h"

#include "../NetItems/PetriNetScene.h"

#include "../NetItems/ArcItem.h"
#include "../NetItems/NetItem.h"

#include <QGraphicsItem>
#include <QMessageBox>

ValidationIssuesModel::ValidationIssuesModel(QObject *parent)
 : QAbstractTableModel(parent) {}

void ValidationIssuesModel::setIssues(const std::vector<PetriEngine::ValidationError> &issues){
	this->beginResetModel();
	_issues.clear();
	_issues = issues;
	this->endResetModel();
}

int ValidationIssuesModel::columnCount(const QModelIndex &parent) const{
	if(parent.isValid())
		return 0;
	return 2;
}

int ValidationIssuesModel::rowCount(const QModelIndex &parent) const{
	if(parent.isValid())
		return 0;
	return _issues.size();
}

void ValidationIssuesModel::clear(){
	this->beginResetModel();
	_issues.clear();
	this->endResetModel();
}

QVariant ValidationIssuesModel::data(const QModelIndex &index, int role) const{
	if(!index.isValid() || role != Qt::DisplayRole)
		return QVariant();	//Return invalid QVariant
	const PetriEngine::ValidationError& issue = _issues[index.row()];

	//If the issue is requested
	if(index.column() == 0){
		if(issue.hasExprError())
			return issue.exprError().toString().c_str();
		return issue.text().c_str();
	}

	//If the location is requested
	if(index.column() == 1){
		QString location;
		if(issue.hasExprError()){
			if(issue.isConditionError())
				location = "In the pre-condition for \"";
			else
				location = "In the post-assignemnt for \"";
			location += issue.startIdentifier().c_str();
			location += "\"";
		}else{
			if(issue.endIdentifier().empty()){
				location += "In \"";
				location +=  issue.startIdentifier().c_str();
				location += "\"";
			}else{
				location = "In arc from \"";
				location += issue.startIdentifier().c_str();
				location += "\" to \"";
				location += issue.endIdentifier().c_str();
				location += "\"";
			}
		}
		return location;
	}

	return QVariant();
}

QVariant ValidationIssuesModel::headerData(int section,
										   Qt::Orientation orientation,
										   int role) const{
	if(role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();
	if(section == 0)
		return tr("Issue");
	else if(section == 1)
		return tr("Location");
	return QVariant();
}

void ValidationIssuesModel::showValidationIssue(PetriNetScene *scene,
												const QModelIndex &index){
	if(!index.isValid())
		return;

	const PetriEngine::ValidationError& issue = _issues[index.row()];

	NetItem* start = scene->findNetItem(issue.startIdentifier().c_str());
	QGraphicsItem* item = start;
	if(!issue.endIdentifier().empty()){
		NetItem* end = scene->findNetItem(issue.endIdentifier().c_str());
		if(start && end)
			item = scene->findArc(start, end);
		else
			item = NULL;
	}

	if(item){
		scene->clearSelection();
		item->setSelected(true);
	}
}
