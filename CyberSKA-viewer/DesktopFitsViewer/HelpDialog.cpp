#include "HelpDialog.h"
#include "ui_HelpDialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::on_indexButton_clicked()
{
//    ui-> webView-> setUrl ( QUrl( "http://www.cyberska.org"));
}

void HelpDialog::on_backButton_clicked()
{
//    ui-> webView-> history ()-> back ();
}

void HelpDialog::on_closeButton_clicked()
{
    close ();
}
