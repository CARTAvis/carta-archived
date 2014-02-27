#include <QCloseEvent>
#include <QAction>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AboutDialog.h"

FvView::FvView(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    // make pixel grid checkboxes exclusive
    pixelGridActionGroup_ = new QActionGroup ( this );
    pixelGridActionGroup_-> addAction ( ui_->pixelGrid5Action );
    pixelGridActionGroup_-> addAction ( ui_->pixelGrid10Action );
    pixelGridActionGroup_-> addAction ( ui_->pixelGrid20Action );
    pixelGridActionGroup_-> addAction ( ui_->pixelGrid25Action );
    pixelGridActionGroup_-> addAction ( ui_->pixelGridOffAction );
    ui_->pixelGridOffAction-> setChecked ( true );

    // coordinates formatting should be radio buttons
    actionGroupCoordinatesFormat_ = new QActionGroup( this);
    actionGroupCoordinatesFormat_-> addAction( ui_-> actionDegrees);
    actionGroupCoordinatesFormat_-> addAction( ui_-> actionSexagecimal);
    ui_-> actionSexagecimal-> setChecked( true);

    // initialize dialogs
    aboutDialog_ = new AboutDialog( this);
    gridOptionsDialog_ = new GridOptionsDialog( this);
//    xProfileDialog_ = new ProfileDialog( "X Profile", "XProfile", this);
//    yProfileDialog_ = new ProfileDialog( "Y Profile", "YProfile", this);
//    zProfileDialog_ = new ProfileDialog( "Z Profile", "ZProfile", this);
    fitsHeaderDialog_ = new FitsHeaderDialog( this);
    screenshotDialog_ = new ScreenshotDialog( this);
    movieDialog_ = new MovieDialog( this);
    changelogDialog_ = new ChangeLogDialog( this);
    helpDialog_ = new HelpDialog( );
    regionStatsDialog_ = new RegionStatsDialog( this);
    gaussianDialog_ = new GaussianDialog( this);
    colormapDialog_ = new ColormapDialog( this);
    // this will be created in controller, as the histogram dialog needs a reference
    // to FitsViewerServer
    histogramDialog_ = 0;

    GetStateManager().XmlStateManager().AddValueChangedHandler("/Colormap/Visible", CSI::Bind(this, &FvView::OnRequestColormap));
    GetStateManager().XmlStateManager().AddValueChangedHandler("/XProfile/Visible", CSI::Bind(this, &FvView::OnRequestXProfile));
    GetStateManager().XmlStateManager().AddValueChangedHandler("/YProfile/Visible", CSI::Bind(this, &FvView::OnRequestYProfile));
    GetStateManager().XmlStateManager().AddValueChangedHandler("/ZProfile/Visible", CSI::Bind(this, &FvView::OnRequestZProfile));

    // GetStateManager().ViewManager().SetFullImageQuality(65);

    colormapDialog_->setVisible( true);
}

Ui::MainWindow *
FvView::ui ()
{
    return ui_;
}

FvView::~FvView()
{
    delete helpDialog_;
    delete ui_;
}

void
FvView::on_actionAbout_triggered()
{
    aboutDialog_-> show();
}

void
FvView::on_actionGridOptions_triggered()
{
    gridOptionsDialog_-> show();
}

/**
  * Callback for toggling the menu bar
  */
void
FvView::on_actionToggleToolbar_triggered()
{
    if( ui_-> mainToolBar-> isHidden())
        ui_-> mainToolBar-> show();
    else
        ui_-> mainToolBar-> hide();
}

void
FvView::on_actionOpenFile_triggered()
{
    QFileDialog fileDialog ( this, "Choose a FITS file to open");
    fileDialog.exec();
}

/**
  * Callback for closing the window
  */
void
FvView::closeEvent(
    QCloseEvent * event
    )
{
    QMessageBox::StandardButton b = QMessageBox::question(
                this, "Confirm", "Are you sure you want to quit?",
                QMessageBox::Yes | QMessageBox::Cancel);
    if( b == QMessageBox::Yes) {
        event-> accept();
        qApp-> quit ();
    } else {
        event-> ignore();
    }
}

void
FvView::on_actionExit_triggered()
{
    this-> close ();
}

void
FvView::on_actionAboutQt_triggered()
{
    qApp-> aboutQt ();
}

void
FvView::on_actionProfileX_changed()
{
//    // update the checkbox if profile dialog is closed
//    // by making the connection unique we can keep this code here instead of constructor
//    connect (xProfileDialog_, SIGNAL( finished(int)), ui_-> actionProfileX, SLOT(toggle()),
//             Qt::UniqueConnection);
//    // set the visibility of the dialog based on the state of the checkbox
//    xProfileDialog_-> setVisible ( ui_-> actionProfileX-> isChecked ());
}

void
FvView::on_actionProfileY_changed()
{
//    // update the checkbox if profile dialog is closed
//    // by making the connection unique we can keep this code here instead of constructor
//    connect (yProfileDialog_, SIGNAL( finished(int)), ui_-> actionProfileY, SLOT(toggle()),
//             Qt::UniqueConnection);
//    // set the visibility of the dialog based on the state of the checkbox
//    yProfileDialog_-> setVisible ( ui_-> actionProfileY-> isChecked ());
}

void
FvView::on_actionProfileZ_changed()
{
//    // update the checkbox if profile dialog is closed
//    // by making the connection unique we can keep this code here instead of constructor
//    connect (zProfileDialog_, SIGNAL( finished(int)), ui_-> actionProfileZ, SLOT(toggle()),
//             Qt::UniqueConnection);
//    // set the visibility of the dialog based on the state of the checkbox
//    zProfileDialog_-> setVisible ( ui_-> actionProfileZ-> isChecked ());
}


void FvView::on_actionFitsHeader_triggered()
{
    fitsHeaderDialog_-> show ();
}

void FvView::on_actionSaveScreenshot_triggered()
{
    screenshotDialog_-> show ();
}

void FvView::on_actionMoviePlayer_triggered()
{
    movieDialog_-> show ();
}

void FvView::on_actionChangelog_triggered()
{
    changelogDialog_-> show ();
}

void FvView::on_actionHelp_triggered()
{
    helpDialog_-> show ();
}

void FvView::on_actionRegionStatistics_triggered()
{
    regionStatsDialog_-> show ();
}

void FvView::on_actionGaussianFitting_triggered()
{
    gaussianDialog_-> show();
}

void FvView::on_actionColormap_triggered()
{
    colormapDialog_-> show();
}

void FvView::on_actionHistogram_triggered()
{
//    histogramDialog_-> show ();
}

// PureWeb Hooks
void FvView::OnRequestXProfile(const CSI::ValueChangedEventArgs& /*args*/)
{
//    CSI::String v = args.NewValue().ValueOr("0");
//    xProfileDialog_-> setVisible(v == "0" ? false : true);
}
void FvView::OnRequestYProfile(const CSI::ValueChangedEventArgs& /*args*/)
{
//    CSI::String v = args.NewValue().ValueOr("0");
//    yProfileDialog_-> setVisible(v == "0" ? false : true);
}
void FvView::OnRequestZProfile(const CSI::ValueChangedEventArgs& /*args*/)
{
//    CSI::String v = args.NewValue().ValueOr("0");
//    zProfileDialog_-> setVisible(v == "0" ? false : true);
}

void FvView::OnRequestColormap(const CSI::ValueChangedEventArgs& args)
{
    CSI::String v = args.NewValue().ValueOr("0");
    colormapDialog_-> setVisible(v == "0" ? false : true);
}
