#include <QFile>
#include <QThread>
#include "DebuggingDialog.h"
#include "ui_DebuggingDialog.h"

DebuggingDialog::DebuggingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebuggingDialog)
{
    ui->setupUi(this);

    ui->debugWidget_->setFont ( QFont( "Courier", 8));
    ui->debugWidget_->setPlainText ( "Debugging below:");

}

DebuggingDialog::~DebuggingDialog()
{
    delete ui;
}

void DebuggingDialog::on_pushButton_clicked()
{
    ui->debugWidget_->clear ();
}

void DebuggingDialog::appendDebugString (const QString & s)
{
    std::cerr << s;

    if( QThread::currentThread () == thread ()) {
        ui->debugWidget_->moveCursor ( QTextCursor::End);
        ui->debugWidget_->insertPlainText ( s);
    }

    if( fname_.isEmpty ()) return;
    QFile f( fname_);
    if( ! f.open( QFile::Append )) return;
    f.write ( s.toAscii ());
    f.close ();
}
