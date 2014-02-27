#ifndef SCREENSHOTDIALOG_H
#define SCREENSHOTDIALOG_H

#include <QDialog>

namespace Ui {
    class ScreenshotDialog;
}

class ScreenshotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScreenshotDialog(QWidget *parent = 0);
    ~ScreenshotDialog();

private:
    Ui::ScreenshotDialog *ui;
};

#endif // SCREENSHOTDIALOG_H
