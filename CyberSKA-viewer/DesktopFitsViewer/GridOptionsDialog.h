#ifndef GRIDOPTIONSDIALOG_H
#define GRIDOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
    class GridOptionsDialog;
}

class GridOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridOptionsDialog(QWidget *parent = 0);
    ~GridOptionsDialog();

private slots:
    void on_gridColorButton_clicked();

    void on_labelColorButton_clicked();

private:
    Ui::GridOptionsDialog *ui;

    QColor gridColor_, labelColor_;
};

#endif // GRIDOPTIONSDIALOG_H
