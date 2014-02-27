#include "ChangeLogDialog.h"
#include "ui_ChangeLogDialog.h"

ChangeLogDialog::ChangeLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeLogDialog)
{
    ui->setupUi(this);
}

ChangeLogDialog::~ChangeLogDialog()
{
    delete ui;
}
