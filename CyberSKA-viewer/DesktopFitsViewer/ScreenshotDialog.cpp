#include "ScreenshotDialog.h"
#include "ui_ScreenshotDialog.h"

ScreenshotDialog::ScreenshotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenshotDialog)
{
    ui->setupUi(this);
}

ScreenshotDialog::~ScreenshotDialog()
{
    delete ui;
}
