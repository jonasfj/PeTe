#ifndef VARIABLEDELEGATE_H
#define VARIABLEDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>

class VariableDelegate : public QItemDelegate{
	Q_OBJECT
public:
	VariableDelegate(QObject* parent=0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
						  const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
					  const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
							  const QModelIndex &index) const;
};



#endif // VARIABLEDELEGATE_H
