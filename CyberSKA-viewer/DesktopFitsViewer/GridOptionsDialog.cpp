#include <QColorDialog>
#include <QPushButton>
#include <QPainter>

#include "GridOptionsDialog.h"
#include "ui_GridOptionsDialog.h"

// set icon to be a solid color
static void setColorIcon( QPushButton * button, QColor color)
{
    QPixmap pixmap( 32, 32);
    pixmap.fill ( color);
    QPainter p( & pixmap);
    p.setPen ( QColor( "black"));
    p.drawRect ( pixmap.rect ());
    button-> setIcon ( pixmap);
}

GridOptionsDialog::GridOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridOptionsDialog)
{
    ui->setupUi(this);
    labelColor_ = QColor( "white");
    gridColor_ = QColor( "yellow");

    setColorIcon ( ui-> gridColorButton, gridColor_);
    setColorIcon ( ui-> labelColorButton, labelColor_);
}

GridOptionsDialog::~GridOptionsDialog()
{
    delete ui;
}

void GridOptionsDialog::on_gridColorButton_clicked()
{
    QColorDialog cd;
    int res = cd.exec ();
    if( res == QDialog::Rejected) return;
    gridColor_ = cd.selectedColor ();
    setColorIcon ( ui-> gridColorButton, gridColor_);
}

void GridOptionsDialog::on_labelColorButton_clicked()
{
    QColorDialog cd;
    int res = cd.exec ();
    if( res == QDialog::Rejected) return;
    labelColor_ = cd.selectedColor ();
    setColorIcon ( ui-> labelColorButton, labelColor_);
}
