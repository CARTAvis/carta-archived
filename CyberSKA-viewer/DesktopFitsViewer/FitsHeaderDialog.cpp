#include <QTextEdit>

#include "FitsHeaderDialog.h"
#include "ui_FitsHeaderDialog.h"


FitsHeaderDialog::FitsHeaderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FitsHeaderDialog)
{
    ui->setupUi(this);

    m_headerVar.reset( new GlobalState::StringVar( "/FITSHeader"));
}

FitsHeaderDialog::~FitsHeaderDialog()
{
    delete ui;
}

void FitsHeaderDialog::setHeaderLines( const QStringList & lines)
{
//    pwset("/FITSHeader", lines.join("\n"));
    m_headerVar-> set( lines.join("\n"));
}
