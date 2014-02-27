#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>

#include "AboutDialog.h"
#include "GridOptionsDialog.h"
//#include "ProfileDialog.h"
#include "FitsHeaderDialog.h"
#include "ScreenshotDialog.h"
#include "MovieDialog.h"
#include "ChangeLogDialog.h"
#include "HelpDialog.h"
#include "RegionStatsDialog.h"
#include "GaussianDialog.h"
#include "ColormapDialog.h"
#include "HistogramDialog.h"

namespace Ui {
    class MainWindow;
}

class FvView : public QMainWindow
{
    Q_OBJECT

public:
    explicit FvView(QWidget *parent = 0);
    ~FvView();
    Ui::MainWindow * ui();

private slots:
    void on_actionAbout_triggered();
    void on_actionGridOptions_triggered();
    void on_actionToggleToolbar_triggered();
    void on_actionOpenFile_triggered();
    void on_actionExit_triggered();
    void on_actionAboutQt_triggered();
    void on_actionProfileX_changed();
    void on_actionProfileY_changed();
    void on_actionProfileZ_changed();
    void on_actionFitsHeader_triggered();
    void on_actionSaveScreenshot_triggered();
    void on_actionMoviePlayer_triggered();
    void on_actionChangelog_triggered();
    void on_actionHelp_triggered();
    void on_actionRegionStatistics_triggered();
    void on_actionGaussianFitting_triggered();
    void on_actionColormap_triggered();
    void on_actionHistogram_triggered();

protected:
    Ui::MainWindow * ui_;
    QActionGroup * pixelGridActionGroup_, * actionGroupCoordinatesFormat_;

    // dialogs
    AboutDialog * aboutDialog_;
    GridOptionsDialog * gridOptionsDialog_;
//    ProfileDialog * xProfileDialog_, * yProfileDialog_, * zProfileDialog_;
    FitsHeaderDialog * fitsHeaderDialog_;
    ScreenshotDialog * screenshotDialog_;
    MovieDialog * movieDialog_;
    ChangeLogDialog * changelogDialog_;
    HelpDialog * helpDialog_;
    RegionStatsDialog * regionStatsDialog_;
    GaussianDialog * gaussianDialog_;
    ColormapDialog * colormapDialog_;
    HistogramDialog * histogramDialog_;

    friend class FvController; // TODO: don't be lazy

    // trapped close event - to be able to show confirmation dialog
    virtual void closeEvent (QCloseEvent * ) ;

    // PureWeb Hooks
    void OnRequestXProfile(const CSI::ValueChangedEventArgs& args);
    void OnRequestYProfile(const CSI::ValueChangedEventArgs& args);
    void OnRequestZProfile(const CSI::ValueChangedEventArgs& args);
    void OnRequestColormap(const CSI::ValueChangedEventArgs& args);

};

#endif // MAINWINDOW_H
