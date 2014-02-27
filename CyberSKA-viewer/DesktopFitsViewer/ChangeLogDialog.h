#ifndef CHANGELOGDIALOG_H
#define CHANGELOGDIALOG_H

#include <QDialog>

namespace Ui {
    class ChangeLogDialog;
}

class ChangeLogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeLogDialog(QWidget *parent = 0);
    ~ChangeLogDialog();

private:
    Ui::ChangeLogDialog *ui;
};

#endif // CHANGELOGDIALOG_H
