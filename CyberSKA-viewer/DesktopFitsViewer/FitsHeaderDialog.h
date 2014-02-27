#ifndef FITSHEADERDIALOG_H
#define FITSHEADERDIALOG_H

#include <QDialog>
#include <memory>
#include "GlobalState.h"

namespace Ui {
    class FitsHeaderDialog;
}

class FitsHeaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FitsHeaderDialog(QWidget *parent = 0);
    ~FitsHeaderDialog();

public slots:
    void setHeaderLines( const QStringList & lines);

protected:
    Ui::FitsHeaderDialog *ui;

    std::unique_ptr< GlobalState::StringVar > m_headerVar;
};

#endif // FITSHEADERDIALOG_H
