#ifndef EDITARCDIALOG_H
#define EDITARCDIALOG_H

#include <QDialog>

namespace Ui {
    class EditArcDialog;
}

class EditArcDialog : public QDialog
{
    Q_OBJECT

public:
	explicit EditArcDialog(QWidget *parent = 0);
    ~EditArcDialog();
	void setInfo(const QString& info);
	int weight() const;
	void setWeight(int weight);
private:
    Ui::EditArcDialog *ui;
};

#endif // EDITARCDIALOG_H
