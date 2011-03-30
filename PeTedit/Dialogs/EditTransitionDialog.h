#ifndef EDITTRANSITIONDIALOG_H
#define EDITTRANSITIONDIALOG_H

#include <QDialog>

namespace Ui {
    class EditTransitionDialog;
}

class EditTransitionDialog : public QDialog
{
    Q_OBJECT

public:

	QString name() const;
	void setName(QString name);

	void setIdentifiers(const QStringList& places, const QStringList& variables);

	QString preConditions() const;
	void setPreConditions(QString conditions);

	QString postConditions() const;
	void setPostConditions(QString conditions);

    explicit EditTransitionDialog(QWidget *parent = 0);
    ~EditTransitionDialog();

private:
	QString _name;
    Ui::EditTransitionDialog *ui;
};

#endif // EDITTRANSITIONDIALOG_H
