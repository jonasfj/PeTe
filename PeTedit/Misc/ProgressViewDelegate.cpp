#include "ProgressViewDelegate.h"
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
	opt.text = QString().sprintf("%d%%", opt.progress);

	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &opt, painter);
}

