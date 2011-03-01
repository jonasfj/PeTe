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

    explicit EditTransitionDialog(QWidget *parent = 0);
    ~EditTransitionDialog();

private:
	QString _name;
    Ui::EditTransitionDialog *ui;
};

#endif // EDITTRANSITIONDIALOG_H
