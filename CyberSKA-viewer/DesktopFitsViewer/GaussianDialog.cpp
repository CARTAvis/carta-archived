#include "GaussianDialog.h"
#include "ui_GaussianDialog.h"

GaussianDialog::GaussianDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GaussianDialog)
{
    ui->setupUi(this);
}

GaussianDialog::~GaussianDialog()
{
    delete ui;
}
