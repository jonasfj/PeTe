#ifndef PROGRESSVIEWDELEGATE_H
#define PROGRESSVIEWDELEGATE_H

#include <QItemDelegate>

/** Delegate that shows items as progress bars */
class ProgressViewDelegate : public QItemDelegate
{
    Q_OBJECT
public:
	explicit ProgressViewDelegate(QObject *parent = 0);

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:

public slots:

};

#endif // PROGRESSVIEWDELEGATE_H
