#ifndef REGIONSTATSDIALOG_H
#define REGIONSTATSDIALOG_H

#include <QDialog>

namespace Ui {
    class RegionStatsDialog;
}

class RegionStatsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegionStatsDialog(QWidget *parent = 0);
    ~RegionStatsDialog();

private:
    Ui::RegionStatsDialog *ui;
};

#endif // REGIONSTATSDIALOG_H
