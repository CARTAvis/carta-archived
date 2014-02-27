#ifndef DEBUGGINGDIALOG_H
#define DEBUGGINGDIALOG_H

#include <QDialog>
#include <QString>
#include "FitsViewerLib/common.h"

namespace Ui {
    class DebuggingDialog;
}

class DebuggingDialog : public QDialog, public Debug::DebugReceiver
{
    Q_OBJECT

public:
    explicit DebuggingDialog(QWidget *parent = 0);
    ~DebuggingDialog();

private slots:
    void on_pushButton_clicked();

public:
    Ui::DebuggingDialog *ui;

    void appendDebugString (const QString &);
    void setOutputFile( QString fname) { fname_ = fname; }

    QString fname_;
};

#endif // DEBUGGINGDIALOG_H
