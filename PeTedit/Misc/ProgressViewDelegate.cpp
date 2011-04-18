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
