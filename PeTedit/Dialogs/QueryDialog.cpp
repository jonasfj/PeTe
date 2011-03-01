#include "QueryDialog.h"
#include "ui_QueryDialog.h"

QueryDialog::QueryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueryDialog)
{
    ui->setupUi(this);
}

QueryDialog::~QueryDialog()
{
    delete ui;
}
