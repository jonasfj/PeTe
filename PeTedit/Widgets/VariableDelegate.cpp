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
#include "VariableDelegate.h"
#include <QtGui>


VariableDelegate::VariableDelegate(QObject *parent):QItemDelegate(parent){

}

QWidget* VariableDelegate::createEditor(QWidget *parent,
										const QStyleOptionViewItem&,
										const QModelIndex &index) const{

	if(index.column() == 0){
		QLineEdit* editor = new QLineEdit(parent);
		return editor;
	} else {
		QSpinBox* editor = new QSpinBox(parent);
		editor->setMinimum(0);
		editor->setMaximum(100);
		return editor;
	}
}

void VariableDelegate::setEditorData(QWidget *editor,
									 const QModelIndex &index) const{
	if(index.column() == 0) {
		QString value = index.model()->data(index,Qt::EditRole).toString();
		QLineEdit* txt = static_cast<QLineEdit*>(editor);
		txt->setText(value);
	}else{
		int value = index.model()->data(index, Qt::EditRole).toInt();
		QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
		spinBox->setValue(value);

		int max = 100;
		// Value may not be larger than range
		if(index.column() == 1){
			QModelIndex idx = index.model()->index(index.row(),index.column()+1);
			max = index.model()->data(idx,Qt::EditRole).toInt();
		}
		spinBox->setMaximum(max);
	}
}

void VariableDelegate::setModelData(QWidget *editor,
									QAbstractItemModel *model,
									const QModelIndex &index) const {
	if(index.column() == 0){
		QLineEdit* txt = static_cast<QLineEdit*>(editor);
		QString value = txt->text();
		model->setData(index, value, Qt::EditRole);
	}else{
		QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
		spinBox->interpretText();
		int value = spinBox->value();
		// Make sure new range is not lower than value
		if(index.column() == 2){
			QModelIndex idx = index.model()->index(index.row(),index.column()-1);
			int lowest = index.model()->data(idx,Qt::EditRole).toInt();
			if(value < lowest)
				value = lowest;
		}
		model->setData(index,value,Qt::EditRole);
	}

}

void VariableDelegate::updateEditorGeometry(QWidget *editor,
											const QStyleOptionViewItem &option,
											const QModelIndex&) const{
	editor->setGeometry(option.rect);
}
