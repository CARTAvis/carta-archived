#include "RegionStatsDialog.h"
#include "ui_RegionStatsDialog.h"

RegionStatsDialog::RegionStatsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegionStatsDialog)
{
    ui->setupUi(this);
}

RegionStatsDialog::~RegionStatsDialog()
{
    delete ui;
}
