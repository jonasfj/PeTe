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
#include "ProgressViewDelegate.h"
#include "CustomDataRoles.h"

#include <QtGui>


ProgressViewDelegate::ProgressViewDelegate(QObject *parent) : QItemDelegate(parent) {}

void ProgressViewDelegate::paint(QPainter *painter,
								 const QStyleOptionViewItem &option,
								 const QModelIndex &index) const{
	if(index.data().type() != QVariant::Double){
		QItemDelegate::paint(painter, option, index);
		return;
	}

	QStyleOptionProgressBar opt;
	opt.state = QStyle::State_Enabled;
	opt.direction = QApplication::layoutDirection();
	opt.rect = option.rect;
	opt.fontMetrics = QApplication::fontMetrics();
	opt.minimum = 0;
	opt.maximum = 100;
	opt.textAlignment = Qt::AlignCenter;
	opt.textVisible = true;
	opt.progress = index.data().toDouble() * 100;

	QVariant ptext = index.data(DataRoles::ProgressText);
	if(ptext.isValid() && !ptext.toString().isEmpty())
		opt.text = ptext.toString();
	else
		opt.text = QString().sprintf("%d%%", opt.progress);

	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &opt, painter);
}

QSize ProgressViewDelegate::sizeHint(const QStyleOptionViewItem &option,
									 const QModelIndex &index) const{
	if(index.data().type() != QVariant::Double)
		return QItemDelegate::sizeHint(option, index);

	QStyleOptionProgressBar opt;
	opt.state = QStyle::State_Enabled;
	opt.direction = QApplication::layoutDirection();
	opt.rect = option.rect;
	opt.fontMetrics = QApplication::fontMetrics();
	opt.minimum = 0;
	opt.maximum = 100;
	opt.textAlignment = Qt::AlignCenter;
	opt.textVisible = true;
	opt.progress = index.data().toDouble() * 100;

	QVariant ptext = index.data(DataRoles::ProgressText);
	if(ptext.isValid() && !ptext.toString().isEmpty())
		opt.text = ptext.toString();
	else
		opt.text = QString().sprintf("%d%%", opt.progress);

	QStyle* style = QApplication::style();
	QRect text = style->itemTextRect(option.fontMetrics,
									 option.rect,
									 option.displayAlignment,
									 true,
									 opt.text);
	text.adjust(-5, 0, 5, 0);
	QSize size = style->sizeFromContents(QStyle::CT_ProgressBar, &opt, text.size());

	return size;
}
