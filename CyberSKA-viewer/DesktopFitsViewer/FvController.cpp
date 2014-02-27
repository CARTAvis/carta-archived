#include "FvController.h"
#include "ProfileController.h"
#include <limits>

#include <cmath>

#include <QStatusBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMainWindow>
#include <QDebug>
#include <fstream>
#include <string>
#include <QPainter>
#include <QSettings>
#include <functional>

#include "ui_MainWindow.h"
#include "FitsViewerLib/common.h"

#include "PureWeb.h"
#include "FitsViewerLib/ProfileService.h"
#include "FileBrowserService.h"
#include "CoordinatesLib/SkyGridPlotter.h"
#include "FitsViewerLib/Optimization/Gauss2d.h"
#include "GlobalSettings.h"
#include "GlobalState.h"

//#define QSTRING(x) QString(x.ToAscii().begin())

// helper function to help us create bindings for global variables
template <class T>
static void binder(
        FvController * th,
        std::unique_ptr<GlobalState::TypedVariable<T> > & ptr,
        const QString & prefix,
        const QString & name)
{
    ptr.reset( new GlobalState::TypedVariable<T>(prefix + name));
    std::function<void()> cb = std::bind(&FvController::globalVariableCB, th, name);
    ptr->addCallback( cb);
}


using namespace CSI;
using namespace CSI::PureWeb;
using namespace CSI::PureWeb::Server;

// constructor
FvController::FvController (
        QStringList /*arguments*/
        ) :
    QObject(0),
    timeoutMonitor_(this),
    m_pwViewName("MainView")
{
    dbg(1) << "FvController constructor running\n";

    //    RaiLib::print_trace( std::cerr);

    // initialize rendering parameters
    m_drawOrigin = QPointF( 0, 0);
    m_drawWidth = 1;
    m_currentFrame = 0;
    m_isCursorFrozen = false;
    m_isFileLoaded = false;
    m_isRepaintNeeded = true;
    m_lastRepaintId = 0;
    m_gridVisible = false;

    // create sub-controllers
    m_profileController = new ProfileController(this);
    m_rsManager = new RegionStatsService::Manager(this);
    m_g2dfitManager = new Gaussian2dFitService::Manager(this);
}

// destructor
FvController::~FvController ()
{
    GetStateManager().ViewManager().UnregisterView(m_pwViewName.toStdString());
    delete m_gui;

}

// start the object
// - render UI
// - connect GUI signals to controller SLOTs
// - render the default frame
void
FvController::start ()
{
    m_gui = new FvView( 0);
    m_gui->histogramDialog_ = new HistogramDialog( m_gui, m_fvs);
    m_gui-> setWindowTitle ( "FITS Viewer: " + m_title);
    m_gui-> show ();

    CSI::PureWeb::Server::ViewImageFormat viewImageFormat;
    viewImageFormat.PixelFormat = CSI::PureWeb::PixelFormat::Rgb24;
    viewImageFormat.ScanLineOrder = CSI::PureWeb::ScanLineOrder::TopDown;
    viewImageFormat.Alignment = 4;

    GetStateManager().ViewManager().RegisterView(m_pwViewName.toStdString(), this);
    GetStateManager().ViewManager().SetViewImageFormat(
                m_pwViewName.toStdString(), viewImageFormat);

//    dbg(1) << "Min/max quality = " << GetStateManager ().ViewManager ().MinImageQuality ()
//           << " / " << GetStateManager ().ViewManager ().MaxImageQuality () << "\n";
//    dbg(1) << "Current interactive/full quality = "
//           << GetStateManager().ViewManager().InteractiveImageQuality()
//           << " / "
//           << GetStateManager().ViewManager().FullImageQuality()
//           << "\n";
    int q = clamp( 65,
                   GetStateManager ().ViewManager ().MinImageQuality (),
                   GetStateManager ().ViewManager ().MaxImageQuality ());
//    dbg(1) << "Setting quality to " << q << "\n";
    GetStateManager ().ViewManager ().SetInteractiveImageQuality ( q);
    GetStateManager ().ViewManager ().SetFullImageQuality ( GetStateManager().ViewManager().MaxImageQuality());
    GetStateManager ().ViewManager ().SetViewInteracting ( m_pwViewName.toStdString(), true);

//    dbg(1) << "Current interactive/full quality = "
//           << GetStateManager().ViewManager().InteractiveImageQuality()
//           << " / "
//           << GetStateManager().ViewManager().FullImageQuality()
//           << "\n";

    GetStateManager().CommandManager().AddUiHandler(
                "LoadFileShortcut", CSI::Bind(this, &FvController::loadFileShortcutCB));
    GetStateManager().CommandManager().AddUiHandler(
                "SetPixelZoom", CSI::Bind(this, &FvController::OnSetPixelZoom));
    GetStateManager().CommandManager().AddUiHandler(
                "SetSliderZoom", CSI::Bind(this, &FvController::OnSetSliderZoom));
    GetStateManager().CommandManager().AddUiHandler(
                "QuitApplication", CSI::Bind(this, &FvController::OnQuitApplication));
    GetStateManager().CommandManager().AddUiHandler(
                "crosshair", CSI::Bind(this, &FvController::OnCrosshair ));
    GetStateManager().CommandManager().AddUiHandler(
                "MainView-click", CSI::Bind(this, &FvController::OnCanvasClick ));
    GetStateManager().CommandManager().AddUiHandler(
                "MainView-rectangle", CSI::Bind(this, &FvController::OnCanvasRectangle ));
    GetStateManager().CommandManager().AddUiHandler(
                "MainView-keyDown", CSI::Bind(this, &FvController::OnCanvasKeyDown ));
    GetStateManager().CommandManager().AddUiHandler(
                "MainView-wheel", CSI::Bind(this, &FvController::OnCanvasWheel ));
    GetStateManager().CommandManager().AddUiHandler(
                "debug", CSI::Bind(this, &FvController::OnClientDebug ));

    GetStateManager().XmlStateManager().AddValueChangedHandler(
                "/requests/set-rs-region", CSI::Bind(this, &FvController::regionStatsRequestCB));
    GetStateManager().XmlStateManager().AddValueChangedHandler(
                "/requests/set-g2dfit-region", CSI::Bind(this, &FvController::g2dFitRequestCB));
    GetStateManager().XmlStateManager().AddValueChangedHandler(
                "/requests/plot3d/setRegion", CSI::Bind(this, &FvController::threeDplotRequestCB));
    GetStateManager().XmlStateManager().AddValueChangedHandler(
                "/requests/setQuality", CSI::Bind(this, &FvController::qualityRequestCB));
    GetStateManager().CommandManager().AddUiHandler(
                "SetWCS", CSI::Bind(this, &FvController::setWCSCommandCB ));
    GetStateManager().XmlStateManager().AddValueChangedHandler(
                "/requests/mainwindow-cursor",
                CSI::Bind(this, &FvController::onCanvasMouseMove));
    GetStateManager().XmlStateManager().AddValueChangedHandler(
                ("/requests/viewUpdated/" + m_pwViewName).toStdString(),
                CSI::Bind(this, &FvController::onViewUpdated));

    // connect signals/slots
    connect ( m_gui->ui ()->canvasWidget, SIGNAL(mousePress(QMouseEvent*)),
              this, SLOT( canvasMousePressCB(QMouseEvent*)));
    connect ( m_gui->ui ()->canvasWidget, SIGNAL(mouseRelease(QMouseEvent*)),
              this, SLOT( canvasMouseReleaseCB(QMouseEvent*)));
    connect ( m_gui->ui ()->canvasWidget, SIGNAL(mouseMove(QMouseEvent*)),
              this, SLOT( canvasMouseMoveCB(QMouseEvent*)));
    connect ( m_gui->ui ()->canvasWidget, SIGNAL(mouseWheel(QWheelEvent*)),
              this, SLOT( canvasMouseWheelCB(QWheelEvent*)));
    connect ( m_gui->ui ()->canvasWidget, SIGNAL(keyPress(QKeyEvent *)),
              this, SLOT( canvasKeyPressCB(QKeyEvent*)));
    connect ( m_gui->movieDialog_, SIGNAL(frameRequested(int)),
              this, SLOT(requestFrame(int)));
    connect ( m_profileController, SIGNAL(requestFrame(int)), SLOT(requestFrame(int)));
    connect ( this, SIGNAL( mainViewIsSynchronized()),
              m_gui->movieDialog_, SLOT( mainViewIsSynchronizedCB()));
    connect( m_gui->histogramDialog_, SIGNAL(valuesChanged(double,double)),
             this, SLOT(histogramCB(double,double)));
    connect( m_gui->colormapDialog_, SIGNAL(colormapChanged(ColormapFunction)),
             this, SLOT(colormapCB(ColormapFunction)));

    //    connect ( fvView_->ui ()->actionDBG, SIGNAL(triggered()),
    //              this, SLOT(loadFromCommandLine()));

    connect ( & requestedFrameTimer_, SIGNAL(timeout()), SLOT(requestedFrameTimerCB()));

    connect ( m_gui->ui ()->zoomSlider, SIGNAL(valueChanged(int)), SLOT(zoomSliderCB(int)));

    m_gui->colormapDialog_-> setNamedColormap ( "Heat");

    //    if( getenv ("RAIDEBUGNOW")) {
    //        loadFileAuto( FitsFileLocation::fromCommandLine (m_commandLine));
    //    } else {
    //        //        loadFileAuto( FitsFileLocation::fromCommandLine (m_commandLine));
    //    }

    setCursorFrozen ( false);

    // start the timeout controller
    timeoutMonitor_.start ();

    // start the file browser service
    m_fileBrowserService = QSharedPointer<FileBrowserService>(
                new FileBrowserService( this));
    connect( m_fileBrowserService.data (), SIGNAL(openFile(QString)),
             this, SLOT( fileBrowserLoadCB(QString)));

    // connect the region stats service to our callback
    connect( m_rsManager, SIGNAL(progress(RegionStatsService::ResultsRS)),
             this, SLOT(regionStatsServiceCB(RegionStatsService::ResultsRS)));
    connect( m_rsManager, SIGNAL(done(RegionStatsService::ResultsRS)),
             this, SLOT(regionStatsServiceCB(RegionStatsService::ResultsRS)));

    // connect the gaussian 2d fitter service to our callback
    connect( m_g2dfitManager, SIGNAL(progress(Gaussian2dFitService::ResultsG2dFit)),
             this, SLOT(gaussian2dFitServiceCB(Gaussian2dFitService::ResultsG2dFit)));
    connect( m_g2dfitManager, SIGNAL(done(Gaussian2dFitService::ResultsG2dFit)),
             this, SLOT(gaussian2dFitServiceCB(Gaussian2dFitService::ResultsG2dFit)));

    // listen for colormap changes
    connect( & m_fvs, SIGNAL(cachedColormapChanged()),
             this, SLOT(cachedColormapChangedCB()));

    // read the file shortcuts and let the client know what they are
    {
        dbg(1) << "Scanning shortcuts:\n";
        QTime t; t.restart();
        QSettings qset( QDir::homePath () + "/.sfviewer.config", QSettings::IniFormat);
        qset.beginGroup( "fileShortcuts");
        for( int i = 0 ; i < 10000 ; i ++ ) {
            QString prefix = QString("%1/").arg(i);
            if( ! qset.contains( prefix + "path"))
                continue;
            std::string path = qset.value( prefix + "path").toString().toStdString();
            std::string title = qset.value( prefix + "title", "").toString().toStdString();
            if( title == "") title = QFileInfo(path.c_str()).baseName().toStdString();
            pwset( QString( "/fileShortcuts/file%1").arg(m_fileShortcuts.size()), title);
            m_fileShortcuts.push_back( path);
        }
        dbg(1) << "  Found " << m_fileShortcuts.size() << " file shortcuts\n";
        pwsetdc( "/fileShortcuts/num", m_fileShortcuts.size());
        qset.endGroup();
        dbg(1) << "  Shortcuts processed in " << t.elapsed() << "ms\n";
    }

    // read the elgg FITS hacks
    {
        dbg(1) << "Scanning elgg hacks:\n";
        QTime t; t.restart();
        QSettings qset( QDir::homePath () + "/.sfviewer.config", QSettings::IniFormat);
        qset.beginGroup( "elggFitsHacks");
        for( int i = 0 ; i < 10000 ; i ++ ) {
            QString prefix = QString("%1/").arg(i);
            if( ! qset.contains( prefix + "path"))
                continue;
            std::string path = qset.value( prefix + "path").toString().toStdString();
            std::string title = qset.value( prefix + "title", "").toString().toStdString();
            if( title == "") title = QFileInfo(path.c_str()).baseName().toStdString();
            m_elggHacks[i] = path;
        }
        dbg(1) << "  Found " << m_elggHacks.size() << " file shortcuts\n";
        qset.endGroup();
        dbg(1) << "  hacks processed in " << t.elapsed() << "ms\n";
    }

    // shared variables
    m_vars.iZoomInit.reset( new GlobalState::VDVar( "/mainWindow/iZoom-init"));
    m_vars.iZoomRequest.reset( new GlobalState::DoubleVar( "/mainWindow/iZoom-set"));

    // pureweb bug: if we don't initialize these, the changes might be received out of
    // order...
    m_vars.iZoomInit-> set({ 0, 0});
    m_vars.iZoomRequest-> set( 0);

    std::function<void()> cb;
    cb = std::bind( & FvController::globalVariableCB, this, "iZoomInit");
    m_vars.iZoomInit-> addCallback( cb);
    cb = std::bind( & FvController::globalVariableCB, this, "iZoomRequest");
    m_vars.iZoomRequest-> addCallback( cb);

    // commands
    {
//        GlobalState::Command::CallbackSignature cb = std::bind( & FvController::globalCommandCB, this, std::placeholders::_1);
//        GlobalState::Command::addCallback(
//                    "mainWindow.iZoom.setInitialPoint", "iZoomInit",
//                    cb);
    }
    canvasRepaint ();
}

void FvController::setDefaultFileToOpen(const FitsFileLocation &loc)
{
    m_defaultImageLocation = loc;
}

void
FvController::setCursorFrozen (bool flag)
{
    // remember the setting
    m_isCursorFrozen = flag;

    // if cursor is getting unfrozen, set the fits position of the cursor based on
    // the last known position of the mouse
    if( ! flag) {
        m_lastCursorFitsPosition = mouse2fitsi ( lastKnownMousePosition ());
    }

    // update the display
    cursorChangedUpdate ();
    canvasRepaint ();
}

void
FvController::toggleCursorFrozen ()
{
    setCursorFrozen ( ! isCursorFrozen ());
}

void
FvController::OnCrosshair (Guid /*sessionid*/, Typeless /*command*/, Typeless /*responses*/)
{

    ////    s_dataloaded = true;
    //    crosshairPt_ = QPointF(
    //                command["/x"].As<double>(),
    //                command["/y"].As<double>());

    ////    debug() << "crosshair " << x << " , " << y << "\n";

    //    canvasRepaint ();

}

// center on screen point
void
FvController::centerScreenPoint (QPointF p)
{
    setDrawOrigin ( drawOrigin().x () - p.x() + m_fvs.size ().width ()/ 2.0,
                    drawOrigin().y () - p.y() + m_fvs.size ().height ()/ 2.0);
}

// center on image point
void
FvController::centerImagePoint (QPointF p)
{
    centerScreenPoint (image2mousef ( p));
}


// center on rectangle in screen/mouse coordinates
void
FvController::zoomScreenRectangle (QPointF p1, QPointF p2)
{
    QPointF ip1 = mouse2imagef ( p1);
    QPointF ip2 = mouse2imagef ( p2);
    zoomImageRectangle ( ip1, ip2);
}

// zoom to a rectangle in image coordinates
void
FvController::zoomImageRectangle (QPointF p1, QPointF p2)
{
    double iw = m_fvs.getHeaderInfo ().naxis1;
    double ww = m_gui->ui ()->canvasWidget->rect ().width ();
    double ih = m_fvs.getHeaderInfo ().naxis2;
    double wh = m_gui->ui ()->canvasWidget->rect ().height ();

    double  x1 = std::min( p1.x (), p2.x ()),
            x2 = std::max( p1.x (), p2.x ()),
            y1 = std::min( p1.y (), p2.y ()),
            y2 = std::max( p1.y (), p2.y ());
    x1 = clamp<double>( x1, 0, iw);
    y1 = clamp<double>( y1, 0, ih);
    x2 = clamp<double>( x2, 0, iw);
    y2 = clamp<double>( y2, 0, ih);

    // figure out new draw width based on aspect ratio of the rectangle vs. window
    double scale;
    if( ww * (y2-y1) > wh * (x2-x1)) {
        // window is wider... scale to fit the height
        scale = (wh * iw) / ( ww * (y2-y1));
    } else {
        // rectangle is wider... scale to fit the width
        scale = iw / ( x2 - x1);
    }
    setDrawWidth ( scale);


    centerImagePoint ( QPointF( (x2+x1)/2.0, (y2+y1)/2));
}

void
FvController::OnCanvasClick (Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
{

    QPointF p( command["/x"].As<double>(),
            command["/y"].As<double>());
    QString button = command["/button"].As<QString>();
    dbg(1) << "button = " << button;
    // if button is left we'll do pan
    if( button.toLower() == "left") {
        centerScreenPoint ( p);
    }
    // on right click we'll unzoom a little bit
    else if( button.toLower() == "right") {
        return;
        // remember the image point and where on the screen it was
        QPointF screenOrig = p;
        QPointF imgOrig = mouse2imagef( screenOrig);
        setDrawWidth ( drawWidth() / 1.5 );

        // make sure that the image original image point maps
        // to the same screen point...
        QPointF screenNew = image2mousef( imgOrig);
        setDrawOrigin( drawOrigin() + screenOrig - screenNew);
    }
}

void
FvController::OnCanvasRectangle (Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
{

    //    s_dataloaded = true;
    QPointF p1( command["/x1"].As<double>(),
            command["/y1"].As<double>());
    QPointF p2( command["/x2"].As<double>(),
            command["/y2"].As<double>());

    zoomScreenRectangle ( p1, p2);
}

void
FvController::OnCanvasKeyDown (Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
{
    int keyCode = command["/keyCode"].As<int>();
    //    int charCode = command["/charCode"].As<int>();
    double mx = command["/mouseX"].As<double>();
    double my = command["/mouseY"].As<double>();

    //    return;

    if( keyCode == 32 ) {
        dbg(1)<<"Space @ " << mx << " " << my << "\n";
        setLastKnownMousePosition( QPointF( mx, my));
        //        lastCursorFitsPosition_ = mouse2fitsi ( lastKnownMousePosition());
        toggleCursorFrozen();
    }
}

void
FvController::OnCanvasWheel (Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
{
    double delta = command["/delta"].ValueOr<double>(0);

    // remember the image point and where on the screen it was
    QPointF screenOrig = lastKnownMousePosition();
    // remember which image point the mouse was pointing to
    QPointF imgOrig = mouse2imagef( screenOrig);


    // apply zoom
//    int inc = 10;
//    int sz = drawWidth2sliderZoom ( drawWidth ());
//    if( delta < 0) sz -= inc; else sz += inc;
    int sz = drawWidth2sliderZoom ( drawWidth ());
    sz += delta * 10;
    sz = clamp( sz, 0, 1000);
    setDrawWidth ( sliderZoom2drawWidth (sz));

    // make sure that the image original image point maps
    // to the same screen point...
    QPointF screenNew = image2mousef( imgOrig);
    setDrawOrigin( drawOrigin() + screenOrig - screenNew);
}



void
FvController::OnClientDebug (Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
{
    std::string text = command["text"].As<std::string>();

    dbg(1) << "Client debug: " << text.c_str () << "\n";
}

void FvController::setWCSCommandCB(Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
{
    bool needCursorChangedUpdate = false;

    Nullable<QString> wcs = command["/wcs"].As<Nullable<QString> >();
    if( wcs.HasValue()) {
        m_fvs.setWCS( wcs.Value());
    }

    Nullable<QString> lineColor = command["/lineColor"].As<Nullable<QString> >();
    if( lineColor.HasValue()) {
        dbg(1) << "Line color updated: " << lineColor.Value() << "\n";
        SkyGridPlotter::setLineColor( lineColor.Value());
    }


    Nullable<QString> textColor = command["/textColor"].As<Nullable<QString> >();
    if( textColor.HasValue()) {
        dbg(1) << "Line color updated: " << textColor.Value() << "\n";
        SkyGridPlotter::setTextColor( textColor.Value());
    }


    Nullable<QString> gridVisible = command["/gridVisible"].As<Nullable<QString> >();
    if( gridVisible.HasValue()) {
        if( gridVisible.Value().toUpper() == "ON")
            m_gridVisible = true;
        else
            m_gridVisible = false;
    }

    minimizeBlankSpace();
    m_fvs.setDrawOrigin( m_drawOrigin);
    if( needCursorChangedUpdate)
        cursorChangedUpdate();
    canvasRepaint();

    // resend region stats (in case coordinate system changed)
    regionStatsServiceCB( rsResults_);
    gaussian2dFitServiceCB( g2dResults_);

    // tell all profile windows the coordinate system changed...
    m_profileController-> updateWcs( m_fvs.wcsHero());
}


/// callback for region stats request
void
FvController::regionStatsRequestCB(
        const CSI::ValueChangedEventArgs& args)
{
    QString val = QString(args.NewValue().ValueOr("").ToAscii().begin());

    QTextStream qs( & val, QIODevice::ReadOnly);
    int left, right, top, bottom;
    qs >> left >> top >> right >> bottom;
    if( qs.status() != qs.Ok || (right-left < 1 && bottom-top < 1)) {
        dbg(1) << "RS request: null\n";
        rsInput_.isNull = true;
    } else {
        dbg(1) << "RS request: " << left << ".." << right << " "
               << top << ".." << bottom << "\n";

        rsInput_.fitsLocation = this-> m_fLocation;
        rsInput_.left = left;
        rsInput_.right = right;
        rsInput_.top = m_fvs.getHeaderInfo().naxis2 - top - 1;
        rsInput_.bottom = m_fvs.getHeaderInfo().naxis2 - bottom - 1;
        rsInput_.currentFrame = m_currentFrame;
        rsInput_.isNull = false;
    }

    m_rsManager->request( rsInput_);
    //    g2dInput_.rsInput = rsInput_;
    //    m_g2dfitManager->request( g2dInput_);
}

/// callback for main window mouse move
void
FvController::onCanvasMouseMove(
        const CSI::ValueChangedEventArgs& args)
{
    QString val = QString(args.NewValue().ValueOr("").ToAscii().begin());

    //    dbg(1) << "mouse move: " << val << "\n";

    QTextStream qs( & val, QIODevice::ReadOnly);
    double x, y;
    qs >> x >> y;
    if( qs.status() != qs.Ok) {
        dbg(1) << "mouse move: null\n";
        return;
    }

    setLastKnownMousePosition( QPointF( x, y));
}

/// callback for gaussian 2d fitting request
void
FvController::g2dFitRequestCB(
        const ValueChangedEventArgs &args)
{
    QString val = QString(args.NewValue().ValueOr("").ToAscii().begin());

    QTextStream qs( & val, QIODevice::ReadOnly);
    int ng, left, right, top, bottom;
    qs >> ng >> left >> top >> right >> bottom;
    if( qs.status() != qs.Ok || (right-left < 1 && bottom-top < 1)) {
        dbg(1) << "G2dfit request: null\n";
        m_g2dInput.isNull = true;
    } else {
        dbg(1) << "G2dfit request: " << ng << ","
               << left << ".." << right << " "
               << top << ".." << bottom << "\n";
        m_g2dInput.isNull = false;
        m_g2dInput.fitsLocation = this-> m_fLocation;
        m_g2dInput.nGaussians = ng;
        m_g2dInput.left = left;
        m_g2dInput.right = right;
        m_g2dInput.top = m_fvs.getHeaderInfo().naxis2 - top - 1;
        m_g2dInput.bottom = m_fvs.getHeaderInfo().naxis2 - bottom - 1;
        m_g2dInput.currentFrame = m_currentFrame;
    }

    m_g2dfitManager->request( m_g2dInput);
}

static void pwtest( std::string pathToDelete)
{
    auto xml = GetStateManager().XmlStateManager();
    std::string path1 = "/Zhistogram/min";
    std::string path2 = "/Zcolormap/max";
    std::string val1 = "123";
    std::string val2 = "abc";
    xml.SetValue( path1, val1);
    xml.SetValue( path2, val2);
    xml.DeleteTree( pathToDelete);
    std::string newVal1 = xml.GetValueAs<std::string>( path1);
    std::string newVal2 = xml.GetValueAs<std::string>( path2);
    auto pad = [] (const std::string & s, uint pad) { auto res = s; while( res.length() < pad) res += " "; return res; };
    dbg(1) << "RemoveTree "
           << pad( pathToDelete, 16)
           << " -> "
           << pad( newVal1, 4)
           << pad( newVal2, 3);
}

/// callback for 3d plot request
void
FvController::threeDplotRequestCB(
        const ValueChangedEventArgs &args)
{
    if( false){
        dbg(1) << "============ pwdeltest ==================";
        pwtest( "/Zcolormap/max/x");
        pwtest( "/Zcolormap/max/");
        pwtest( "/Zcolormap/max");
        pwtest( "/Zcolormap/");
        pwtest( "/Zcolormap");
        pwtest( "/Zmovie");
        pwtest( "/Zmovie/");
        pwtest( "/Zmovie/frames");
        pwtest( "/Zmovie/frames/");
        pwtest( "Zcolormap/max/x");
        pwtest( "Zcolormap/max/");
        pwtest( "Zcolormap/max");
        pwtest( "Zcolormap/");
        pwtest( "Zcolormap");
        pwtest( "Zmovie");
        pwtest( "Zmovie/");
        pwtest( "Zmovie/frames");
        pwtest( "Zmovie/frames/");
        dbg(1) << "=========================================";
    }

    if( false){
        static bool once = true;
        if( once) {
            int nCh = 5000;
            once = false;
            QString randStr;
            for( int i = 0 ; i < 5 ; i ++ ) {
                randStr += QString( " %1").arg( qrand());
            }
            dbg(1) << "populating state tree with " << nCh << " random chunks";
            for(int i = 0 ; i < nCh ; i ++ ) {
                pwset( QString("/Plot3d/Zchunk%1").arg(i), randStr);
            }
        }
    }

    const int MaxChunkLength = 65360 - 30;
//    const int MaxChunkLength = 200 - 30;
//    const int MaxChunkLength = 2147483;

    QString val = QString(args.NewValue().ValueOr("").ToAscii().begin());

    QTextStream qs( & val, QIODevice::ReadOnly);
    int left, right, top, bottom;
    qs >> left >> top >> right >> bottom;

    // clamp the coordinates
    swap_ordered( left, right);
    swap_ordered( top, bottom);
    left = clamp( left, 0, m_fvs.getHeaderInfo().naxis1 - 1);
    right = clamp( right, 0, m_fvs.getHeaderInfo().naxis1 - 1);
    top = clamp( top, 0, m_fvs.getHeaderInfo().naxis2 - 1);
    bottom = clamp( bottom, 0, m_fvs.getHeaderInfo().naxis2 - 1);

    QString prefix = "/Plot3d/";
    if( qs.status() != qs.Ok || right-left < 1 || bottom-top < 1) {
        dbg(1) << "3d plot request: null\n";
        pwset( prefix + "isNull", true);
    } else {
//        static int maxChunks = -1;
        pwdeltree( "Plot3d");
        dbg(1) << "3d plot request: "
               << left << ".." << right << " "
               << top << ".." << bottom << "\n";
        pwset( prefix + "isNull", false);
        pwset( prefix + "nCols", right - left + 1);
        pwset( prefix + "nRows", bottom - top + 1);
        int chunk = 0;
        QString str;
        double localMin = std::numeric_limits<double>::quiet_NaN();
        double localMax = std::numeric_limits<double>::quiet_NaN();
        for( int y = bottom ; y >= top ; y -- ) {
            for( int x = left ; x <= right ; x ++ ) {
                double val = m_fvs.getValue( x, m_fvs.getHeaderInfo ().naxis2 - y - 1, m_currentFrame);
                QString vals;
                if( std::isfinite(val)) {
                    vals = QString::number( val);
                    if( val < localMin || ! std::isfinite( localMin)) localMin = val;
                    if( val > localMax || ! std::isfinite( localMax)) localMax = val;
                } else {
                    vals = "n";
                }
                if( ! str.isEmpty()) str.append( ' ');
                str += vals;
                if( str.length() > MaxChunkLength) {
//                    str = str.trimmed();
                    pwset( QString( "%1dataChunk%2").arg(prefix).arg(chunk), str);
                    chunk ++;
                    str = "";
                }
            }
        }
        pwset( prefix + "localMin", localMin);
        pwset( prefix + "localMax", localMax);
        if( str.length() > 0) {
            str = str.trimmed();
            pwset( QString( "%1dataChunk%2").arg(prefix).arg(chunk), str);
        }
        dbg(1) << "Chunks: " << chunk+1 << "\n";
//        for( int i = chunk + 1 ; i < maxChunks ; i ++ ) {
//            pwset( QString( "%1dataChunk%2").arg(prefix).arg(i), str);
//        }
//        maxChunks = std::max( chunk, maxChunks);
    }
    pwsetdc( prefix + "stamp", qrand());
}


void FvController::qualityRequestCB(const ValueChangedEventArgs &args)
{
    int val = args.NewValue().ConvertOr( 85);

    GetStateManager ().ViewManager ().SetInteractiveImageQuality ( val);
    GetStateManager ().ViewManager ().SetFullImageQuality ( val);
    GetStateManager().ViewManager().RenderViewDeferred( m_pwViewName.toStdString());
}


/// callback for the RegionStatsService::Manager
void FvController::regionStatsServiceCB(RegionStatsService::ResultsRS r)
{
    dbg(1) << "Region stats CB " << r.status() << " "
           << r.width << " x " << r.height
           << " ts:" << r.toString()
           << "\n";

    rsResults_ = r;

    QString beamArea;
    if( ! isnan(r.beamArea))
        beamArea = QString::number( r.beamArea) + " pixels";

    if( r.status() == r.NullInput) {
        double nan = 0.0 / 0.0;
        pwset( "/RS/isNull", 1);
        pwset( "/RS/width", nan);
        pwset( "/RS/height", nan);
        pwset( "/RS/totalPixels", nan);
        pwset( "/RS/nanPixels", nan);
        pwset( "/RS/average", nan);
        pwset( "/RS/sum", nan);
        pwset( "/RS/rms", nan);
        pwset( "/RS/min",nan);
        pwset( "/RS/max", nan);
        pwset( "/RS/bkgLevel", nan);
        pwset( "/RS/sumMinusBkg", nan);
        pwset( "/RS/maxMinusBkg", nan);
//        pwset( "/RS/maxPos/x", nan);
//        pwset( "/RS/maxPos/y", nan);
//        pwset( "/RS/maxPos/extra", "n/a");
        pwset( "/RS/maxPos/line1", "n/a");
        pwset( "/RS/maxPos/line2", "n/a");

        pwset( "/RS/depth", 0);
        pwset( "/RS/currentFrame", nan);
        pwset( "/RS/nFramesComputed", 0);
        pwset( "/RS/totalFluxDensity", nan);
        pwset( "/RS/aboveBackground", nan);
        pwset( "/RS/beamArea", beamArea);
    } else {
        pwset( "/RS/isNull", 0);
        pwset( "/RS/width", r.width);
        pwset( "/RS/height", r.height);
        pwset( "/RS/totalPixels", r.totalPixels);
        pwset( "/RS/nanPixels", r.nanPixels);
        pwset( "/RS/average", QString("%1").arg( m_fvs.formatValue(r.average)));
        pwset( "/RS/sum", QString("%1").arg(m_fvs.formatValue(r.sum)));
        pwset( "/RS/rms", QString("%1").arg(m_fvs.formatValue(r.rms)));
        pwset( "/RS/min", QString("%1").arg(m_fvs.formatValue(r.min)));
        pwset( "/RS/max", QString("%1").arg(m_fvs.formatValue(r.max)));
        pwset( "/RS/bkgLevel", QString("%1").arg(m_fvs.formatValue(r.bkgLevel)));
        pwset( "/RS/sumMinusBkg", QString("%1").arg(m_fvs.formatValue(r.sumMinusBkg)));
        pwset( "/RS/maxMinusBkg", QString("%1").arg(m_fvs.formatValue(r.maxMinusBkg)));
//        pwset( "/RS/maxPos/x", r.maxPos.x());
//        pwset( "/RS/maxPos/y", r.maxPos.y());
//        pwset( "/RS/maxPos/extra", fvs_.formatCoordinate( r.maxPos.x(), r.maxPos.y()));
        pwset( "/RS/maxPos/line1",
               QString("x: %1 y: %2").arg(r.maxPos.x()).arg(r.maxPos.y()));

        // update the WCS formatted position of the max pixel
        pwset( "/RS/maxPos/line2", m_fvs.formatCoordinate( r.maxPos.x(), r.maxPos.y()));

        pwset( "/RS/depth", r.depth);
        pwset( "/RS/currentFrame", r.currentFrame +1);
        pwset( "/RS/nFramesComputed", r.nFramesComputed);
        QString tfd, ab;
        if( ! isnan( r.beamArea)) {
            if( m_fvs.totalFluxDensityUnit() != "") {
                tfd = QString("%1 %2").arg( r.totalFluxDensity).arg(m_fvs.totalFluxDensityUnit());
                ab = QString("%1 %2").arg( r.aboveBackground).arg(m_fvs.totalFluxDensityUnit());
            } else {
                tfd = QString("%1").arg( r.totalFluxDensity);
                ab = QString("%1").arg( r.aboveBackground);
            }
        }
        pwset( "/RS/totalFluxDensity", tfd);
        pwset( "/RS/aboveBackground", ab);
        pwset( "/RS/beamArea", beamArea);

        this->m_profileController->setRegionStatsResults( r);

        /*
        { // send the average array
            std::ostringstream out;
            for( int i = 0 ; i < r.nFramesComputed ; i ++ )
                out << (i ? " " : "") << r.frames[i].average;
            pwset("/RS/a/average", out.str());
        }{ // send the nanPixels array
            std::ostringstream out;
            for( int i = 0 ; i < r.nFramesComputed ; i ++ )
                out << (i ? " " : "") << r.frames[i].nanPixels;
            pwset("/RS/a/nanPixels", out.str());
        }{ // send the min array
            std::ostringstream out;
            for( int i = 0 ; i < r.nFramesComputed ; i ++ )
                out << (i ? " " : "") << r.frames[i].min;
            pwset("/RS/a/min", out.str());
        }{ // send the max array
            std::ostringstream out;
            for( int i = 0 ; i < r.nFramesComputed ; i ++ )
                out << (i ? " " : "") << r.frames[i].max;
            pwset("/RS/a/max", out.str());
        }{ // send the sum array
            std::ostringstream out;
            for( int i = 0 ; i < r.nFramesComputed ; i ++ )
                out << (i ? " " : "") << r.frames[i].sum;
            pwset("/RS/a/sum", out.str());
        }{ // send the rms array
            std::ostringstream out;
            for( int i = 0 ; i < r.nFramesComputed ; i ++ )
                out << (i ? " " : "") << r.frames[i].rms;
            pwset("/RS/a/rms", out.str());
        }{ // send the bkgLevel array
            std::ostringstream out;
            for( int i = 0 ; i < r.nFramesComputed ; i ++ )
                out << (i ? " " : "") << r.frames[i].bkgLevel;
            pwset("/RS/a/bkgLevel", out.str());
        }{ // send the sumMinusBkg array
            std::ostringstream out;
            for( int i = 0 ; i < r.nFramesComputed ; i ++ )
                out << (i ? " " : "") << r.frames[i].sumMinusBkg;
            pwset("/RS/a/sumMinusBkg", out.str());
        }{ // send the maxMinusBkg array
            std::ostringstream out;
            for( int i = 0 ; i < r.nFramesComputed ; i ++ )
                out << (i ? " " : "") << r.frames[i].maxMinusBkg;
            pwset("/RS/a/maxMinusBkg", out.str());
        }
                        */

    }

    // tell client to re-read this
    pwsetdc( "/RS/stamp", 1);
}

/// callback for the gaussian 2d fitter service
void FvController::gaussian2dFitServiceCB(Gaussian2dFitService::ResultsG2dFit r)
{
    dbg(1) << "Gaussian 2d service: "
           << " ts:" << r.toString()
           << "\n";

    {
        QString s; QTextStream o( & s);
        int ng = r.params.size() / 6;
        for( int i = 0 ; i < ng ; i ++ ) {
            Optimization::Gauss2dNiceParams p;
            p = p.convert( & r.params[i*6]);
            o << QString("%1 %2_,_%3_pix %4 %5_x_%6_pix %7&deg; ")
                 //                 .arg( p.amp, 0, 'g', 7)
                 .arg( m_fvs.formatValue( p.amp).replace(' ', '_'))
                 .arg( p.xc, 0, 'f', 3)
                 .arg( p.yc, 0, 'f', 3)
                 .arg( m_fvs.formatCoordinate(p.xc,p.yc).replace(' ', '_'))
                 .arg( p.major, 0, 'f', 3)
                 .arg( p.minor, 0, 'f', 3)
                 .arg( p.angle, 0, 'f', 3);
        }
        //        for( size_t i = 0 ; i < r.params.size() ; i ++ ) {
        //            o << r.params[i];
        //            if( i+1 < r.params.size()) o << " ";
        //        }
        if( r.params.size() >= size_t(ng * 6) && r.params.size() > 0)
            o << QString("%1")
                 .arg(m_fvs.formatValue(r.params[ r.params.size()-1]).replace(' ', '_'));
        s = s.trimmed();

        pwset( "/Gauss2d/params", s);
        pwset( "/Gauss2d/ng", ng);
    }
    if( r.status() == r.Complete)
        pwset( "/Gauss2d/status", "Complete");
    else if( r.status() == r.Partial)
        pwset( "/Gauss2d/status", "Computing + (" + r.info + ")");
    else
        pwset( "/Gauss2d/status", "Error");
//    pwset( "/Gauss2d/chisq", QString("&chi;&sup2;: %1").arg(r.chisq, 0, 'g', 7));
    pwset( "/Gauss2d/chisq", QString("%1").arg(r.chisq, 0, 'g', 7));
    pwset( "/Gauss2d/rms", QString("%1").arg(m_fvs.formatValue(r.rms)));
    pwset( "/Gauss2d/isNull", r.input.isNull ? 1 : 0);
    pwset( "/Gauss2d/frame", r.input.currentFrame+1);

    // tell client to re-read this
    pwsetdc( "/Gauss2d/stamp", 1);

    g2dResults_ = r;
    canvasRepaint();
}



// sets zoom level
//   pzoom = zoom level in %, e.g. 100% means 1 fits pixel will be 1 real pixel
//   pos = screen position around which to zoom

void
FvController::setPixelZoom (double pzoom, QPoint pos)
{
    QPointF refPt = mouse2imagef( pos);
    setDrawWidth ( pixelZoom2drawWidth ( pzoom));
    centerImagePoint ( refPt);
}


// slider based zoom
//   izoom: value in range 0..1000
//          unless it's -1, which will center the image
//   pos: screen position around which to center

void
FvController::setSliderZoom (int izoom, QPoint pos)
{
    dbg(1) << QString("set slider zoom to %1").arg(izoom) << "\n";

    double iw = m_fvs.getHeaderInfo ().naxis1;
    double ih = m_fvs.getHeaderInfo ().naxis2;

    // special case: for zoom == -1, we fit & center the entire image
    if( izoom == -1)
        return zoomImageRectangle ( QPointF(0,0), QPointF( iw, ih));

    // clamp zoom to 0-1000 range
    izoom = clamp( izoom, 0, 1000);

    //    QPointF refPt = mouse2imagef( pos);

    double ww = m_gui->ui ()->canvasWidget->rect ().width ();
    double wh = m_gui->ui ()->canvasWidget->rect ().height ();

    double zoom = (exp( izoom / 1000.0) - 1) / (exp(1.0) - 1);
    zoom *= zoom;
    double minZoom = std::min( ww/iw * 0.9, wh/ih * 0.9);
    double maxZoom = 64;
    zoom = (1-zoom) * minZoom + zoom * maxZoom;

    setPixelZoom ( zoom, pos);

}

void
FvController::zoomSliderCB (int val)
{
    setSliderZoom ( val, m_gui->ui ()->canvasWidget->rect ().center ());
}

void
FvController::OnSetPixelZoom (CSI::Guid /*sessionid*/, CSI::Typeless command, CSI::Typeless /*responses*/)
{
    double zoom = command["/zoom"].As<double>();
    int width = command["/width"].As<int>();
    int height = command["/height"].As<int>();

    dbg(1) << QString("pw-pixel-zoom %1 %2 %3").arg(zoom).arg(width).arg(height) << "\n";

    setPixelZoom ( zoom, m_gui->ui ()->canvasWidget->rect ().center ());
}

void
FvController::OnSetSliderZoom (CSI::Guid /*sessionid*/, CSI::Typeless command, CSI::Typeless /*responses*/)
{
    int izoom = command["/zoom"].As<double>();
    int width = command["/width"].As<int>();
    int height = command["/height"].As<int>();
    dbg(1) << QString("pw-slider-zoom %1 %2 %3").arg(izoom).arg(width).arg(height) << "\n";

    zoomSliderCB ( izoom);
}


void
FvController::OnQuitApplication (CSI::Guid /*sessionid*/, CSI::Typeless /*command*/, CSI::Typeless /*responses*/)
{
    qApp->exit ();
}

//void
//FvController::loadFromCommandLine(void)
//{
//    loadFile ( FitsFileLocation::fromCommandLine (m_commandLine));
//}


// PureWeb: callback for setting a new size
void FvController::SetClientSize(Size clientSize)
{
//    dbg(1) << "SetClientSize " << clientSize.Width << " x " << clientSize.Height;
//    dbg(1) << "  buff is " << buffer_.width() << " x " << buffer_.height();
//    dbg(1) << "  fvs is " << m_fvs.size().width() << " x " << m_fvs.size().height();
//    dbg(1) << "  gui is  " << m_gui->ui ()->canvasWidget->width ()
//           << " x " << m_gui->ui ()->canvasWidget->height ();
//    dbg(1) << "  gui2 is " << m_gui->ui ()->canvasWidget->rect(). width ()
//           << " x " << m_gui->ui ()->canvasWidget->rect(). height ();

//    QPointF p1 = mouse2imagef (QPointF(0,0));
//    QPointF p2 = mouse2imagef (QPointF(m_fvs.size().width()-1, m_fvs.size().height()-1));

    double oldPixelZoom = drawWidth2pixelZoom( drawWidth());
//    dbg(1) << "pixel zoom = " << oldPixelZoom << "\n";
//    QPointF oldCenterImagePt = lastRect.center();
    QPointF oldCenterImagePt = mouse2imagef( QPointF( m_fvs.size().width() / 2.0, m_fvs.size().height() / 2.0));

//    dbg(1) << "centerPt = " << oldCenterImagePt.x() << " " << oldCenterImagePt.y();

    m_gui->ui ()->canvasWidget->resize ( QSize( clientSize.Width, clientSize.Height));

//    dbg(1) << "Asking fvs to set size to " << clientSize.Width
//           << "x" << clientSize.Height << "\n";
    m_fvs.resize ( QSize( clientSize.Width, clientSize.Height));
//    dbg(1) << "Fvs is now " << m_fvs.size().width() << "x"
//           << m_fvs.size().height() << "\n";


    //    zoomImageRectangle( lastRect.topLeft(), lastRect.bottomRight());
    // apply zoom to match the old zoom, and
//    QPointF where = image2mousef( oldCenterImagePt);
//    setPixelZoom( oldPixelZoom, where.toPoint());

//    QPointF refPt = mouse2imagef( pos);

//    // set the zoom to match the old zoom (in pixel size)
    setDrawWidth ( pixelZoom2drawWidth ( oldPixelZoom));

//    // recenter to the same image point
    centerImagePoint ( oldCenterImagePt);

//    zoomScreenRectangle ( image2mousef(p1), image2mousef(p2));


//    dbg(1) << "  buff is " << buffer_.width() << " x " << buffer_.height();
//    dbg(1) << "  fvs is " << m_fvs.size().width() << " x " << m_fvs.size().height() << "\n";
//    dbg(1) << "  gui is  " << m_gui->ui ()->canvasWidget->width ()
//           << " x " << m_gui->ui ()->canvasWidget->height ();
//    dbg(1) << "  gui2 is " << m_gui->ui ()->canvasWidget->rect(). width ()
//           << " x " << m_gui->ui ()->canvasWidget->rect(). height ();

//    QPointF newCenterPt = mouse2imagef( QPointF( m_fvs.size().width() / 2.0, m_fvs.size().height() / 2.0));
//    dbg(1) << "centerPt = " << newCenterPt.x() << " " << newCenterPt.y();
//    dbg(1) << "centerPt = " << newCenterPt;

    // repaint everything
    canvasRepaint();

//    dbg(1) << "SetClientSize done";
}

Size FvController::GetActualSize()
{
    QSize size = m_fvs.size();
    dbg(1) << "Get actual size = " << size.width() << "x" << size.height() << "\n";
    return Size( size.width(), size.height());
//    return Size(buffer_.width(), buffer_.height());
}

// should be called when cursor position is changed
// updates the text labels and profiles
void
FvController::cursorChangedUpdate( void)
{
    if( ! m_isFileLoaded) return;

    //    bool clipOutside = true;

    QPoint fitsPos = m_lastCursorFitsPosition;

    int imgWidth = m_fvs.getHeaderInfo ().naxis1;
    int imgHeight = m_fvs.getHeaderInfo ().naxis2;
    // clip the fits position
    bool isOutside = fitsPos.x () < 1 || fitsPos.x () > imgWidth
            || fitsPos.y () < 1 || fitsPos.y() > imgHeight;

//    QString cursor2str = QString("x: %1\ny: %2").arg(fitsPos.x(),4).arg(fitsPos.y(),4);
    QString cursor2str;
    cursor2str += QString("    X: %1\n").arg(fitsPos.x());
    cursor2str += QString("    Y: %1\n").arg(fitsPos.y());
    cursor2str += QString("Frame: %1/%2")
            .arg(m_currentFrame + 1)
            .arg( m_fvs.getHeaderInfo().totalFrames);

    // TODO: should the x/y offset be adjusted by 0.5?
    QString cursorStr = m_fvs.formatCursor( fitsPos.x()-1, fitsPos.y()-1, m_currentFrame).join("\n");

    QString valueStr = "Value:";
    if( ! isOutside) {
        double value = m_fvs.getValue( fitsPos.x () - 1, fitsPos.y () - 1, m_currentFrame);
        valueStr = "Value: " + m_fvs.formatValue ( value);
    }

    QString frameStr = QString("Frame: %1 / %2")
            .arg(m_currentFrame + 1)
            .arg( m_fvs.getHeaderInfo().totalFrames);

    pwset("/Status/Cursor", cursorStr);
    pwset("/Status/Cursor2", cursor2str);
    pwset("/Status/Value", valueStr);
    pwset("/Status/Frame", frameStr);
    pwset("/Status/WCS", m_fvs.getWCS());
    pwset("/Status/isOutside", isOutside ? "1" : "0");

    // update frozen cursor status
    QPointF pm = fits2mousef( m_lastCursorFitsPosition + QPointF( 0.5, -0.5));

    pwset("/Status/frozenCursor", QString("%1 %2 %3")
          .arg( isCursorFrozen() ? "1" : "0")
          .arg( pm.x())
          .arg( pm.y())
          );
//    if( isCursorFrozen ()) {
//        pwset("/Status/cursorFrozen", "1");
//        QPointF pm = fits2mousef( lastCursorFitsPosition_ + QPointF( 0.5, -0.5));
//        pwset("/Status/mouseX", pm.x());
//        pwset("/Status/mouseY", pm.y());
//    } else {
//        pwset("/Status/cursorFrozen", "0");
//    }

    pwsetdc( "/Status/dc", "dc");




//    QPoint mouseFits = mouse2fitsi( lastKnownMousePosition());
//    QPoint mouseImg = mouse2imagei( lastKnownMousePosition());
    //    dbg(1) << "******* mouseImg *********** "
    //           << mouseFits.x() << " " << mouseFits.y() << " {} "
    //           << mouseImg.x() << " " << mouseImg.y() << "\n";
//    pwset( "/MouseFitsPos", QString("%1 %2 %3").arg(mouseFits.x()).arg(mouseFits.y()).arg(currentFrame_));
//    pwset( "/MouseImgPos", QString("%1 %2 %3").arg(mouseImg.x()).arg(mouseImg.y()).arg(currentFrame_));
//    pwset( "/MousePos", QString("%1 %2").arg(lastKnownMousePosition().x()).arg(lastKnownMousePosition().y()));

    updateProfileDialogs ();

//    updateFrozenCursor();
}
QString FvController::title() const
{
    return m_title;
}

void FvController::globalVariableCB(const QString &name)
{
    // guard internals until constructor is finished
//    if( m_ignoreVarCallbacks) return;

    dbg(1) << "qu gvar " << name << " changed.";
    if( name == "iZoomRequest") {
        double dy = - m_vars.iZoomRequest-> get();
        dbg(1) << "iZoomRequest '" << dy << "'";
        double dw = m_iZoomInitialDrawWidth * std::pow( 1.01, dy);
        setDrawWidth( dw);
        // where is the desired image point now?
        QPointF pt = image2mousef( m_iZoomInitialImagePt);
        // apply the difference
        setDrawOrigin( drawOrigin().x() + (m_iZoomInitialScreenPt.x() - pt.x()),
                       drawOrigin().y() + (m_iZoomInitialScreenPt.y() - pt.y()));
    }
    else if( name == "iZoomInit") {
        GlobalState::VD arr = m_vars.iZoomInit-> get();
        for( auto x : arr) {
            dbg(1) << "x=" << x;
        }
        if( arr.size() != 2) {
            warn() << "iZoomInit received with bad data: " << m_vars.iZoomInit-> getRaw();
        }
        else {
            m_iZoomInitialScreenPt = QPointF( arr[0], arr[1]);
            m_iZoomInitialImagePt = mouse2imagef( m_iZoomInitialScreenPt);
            m_iZoomInitialDrawWidth = drawWidth();
        }

    }
    else {
        warn() << "unhandled var: " << name;
    }

}

QString FvController::globalCommandCB(const GlobalState::Command::CallbackParameter & p)
{
    dbg(1) << "global command: " << p.name << " " << p.value;
    QString val = p.value;
    val.replace( '_', ' ');

    if( p.name == "iZoomInit") {
        GlobalState::VD arr = GlobalState::Converter<GlobalState::VD>::convertStringToVal( val);
        for( auto x : arr) {
            dbg(1) << "x=" << x;
        }
        if( arr.size() != 2) {
            warn() << "iZoomInit received with bad data: " << p.value;
        }
        else {
            m_iZoomInitialScreenPt = QPointF( arr[0], arr[1]);
            m_iZoomInitialImagePt = mouse2imagef( m_iZoomInitialScreenPt);
            m_iZoomInitialDrawWidth = drawWidth();
        }
    }
    else {
        dbg(1) << ConsoleColors::warning()
               << "unknown command: " << p.name
               << ConsoleColors::resetln();
    }
    return QString();
}

void FvController::setTitle(const QString &title)
{
    m_title = title;
    pwset( "/title", m_title);
}


// update PureWeb UI to reflect the status of the frozen cursor
void
FvController::updateFrozenCursor ()
{
    // update PW status
    QString s;
    QTextStream out( & s);


    if( isCursorFrozen ()) {
        QPointF pm = fits2mousef( m_lastCursorFitsPosition + QPointF( 0.5, -0.5));
        QPointF pi( m_lastCursorFitsPosition.x()-1,
                    m_fvs.getHeaderInfo().naxis2 - m_lastCursorFitsPosition.y());
        // calculate the mouse/screen position of the frozen FITS cursor
        out << "yes " << pm.x () << " " << pm.y () << " "
            << m_gui->ui ()->canvasWidget->width () << " "
            << m_gui->ui ()->canvasWidget->height () << " "
            << pi.x() << " " << pi.y();
    } else {
        out << "no -1 -1 -1 -1 -1 -1";
    }
    pwset( "/CursorFrozen", s);

}


// update profile dialogs
void
FvController::updateProfileDialogs ()
{
    if( ! m_isFileLoaded) return;

    int imgWidth = m_fvs.getHeaderInfo ().naxis1;
    int imgHeight = m_fvs.getHeaderInfo ().naxis2;
    QPoint fitsPos = m_lastCursorFitsPosition;

    // clip the fits position
    bool isOutside = fitsPos.x () < 1 || fitsPos.x () > imgWidth
            || fitsPos.y () < 1 || fitsPos.y() > imgHeight;

    if( isOutside) return;

    m_profileController-> setCursor ( fitsPos.x ()-1, fitsPos.y () - 1, m_currentFrame );
}

// update clients with the visible rectangle, and also send them
// updated transformations to/from pixel coordinates
void FvController::sendVisibleRectangle()
{
    if( m_isFileLoaded) {
        double minx = 0, maxx = 1, miny = 0, maxy = 1;
        int imgWidth = m_fvs.getHeaderInfo ().naxis1;
        int imgHeight = m_fvs.getHeaderInfo ().naxis2;

        QPointF bottomLeftFits = mouse2fitsf ( QPointF(0,m_fvs.size ().height ()));
        QPointF topRightFits = mouse2fitsf (
                    QPointF(m_fvs.size ().width (), 0));
        minx = clamp<double>( bottomLeftFits.x()-1, 0, imgWidth-1 );
        maxx = clamp<double>( topRightFits.x()-1, 0, imgWidth-1 );

        miny = clamp<double>( bottomLeftFits.y()-1, 0, imgHeight-1 );
        maxy = clamp<double>( topRightFits.y()-1, 0, imgHeight-1 );

        //        dbg(1) << "Visible x = " << minx << " .. " << maxx << "\n";
        //        dbg(1) << "Visible y = " << miny << " .. " << maxy << "\n";

        pwset( "/VisibleImageRect", QString("%1 %2 %3 %4")
               .arg(minx).arg(maxx).arg(miny).arg(maxy));
        // tell the profile controller as well
        m_profileController-> setVisibleRectFits( minx, maxx, miny, maxy);


        // coordinate transformation
        QPointF p1 = image2mousef( QPoint(0,0));
        QPointF p2 = image2mousef( QPoint(1,1));
        pwset( "/CoordinateTX", QString("%1 %2 %3 %4")
               .arg( p2.x() - p1.x())
               .arg( p1.x())
               .arg( p2.y() - p1.y())
               .arg( p1.y())
               );
    } else {
        pwset( "/VisibleImageRect", QString(""));
        pwset( "/CoordinateTX", QString("1 1 0 0"));
    }

}

void FvController::cachedColormapChangedCB()
{
    const CachedRgbFunction & fnc = m_fvs.getCachedColormap();
    int width = 6; int base = 16; QChar fillc('0');

    QString val;
    for( int i = 0 ; i <= 100 ; i ++ ) {
        double x = i / 100.0;
        auto col = fnc.qrgb( m_histMin * (1-x) + m_histMax * (x)) & 0xffffff;
        QString hex = QString( "%1").arg( col, width, base, fillc);
        val += hex;
    }

    // let the client know the cached colormap has changed
    pwset( "/cachedColormap/data", val);
    pwset( "/cachedColormap/histMin", m_histMin);
    pwset( "/cachedColormap/histMax", m_histMax);
    pwset( "/cachedColormap/stamp", qrand());
}

// set the last known mouse position
void
FvController::setLastKnownMousePosition (const QPointF & p)
{
    lastKnownMousePosition_ = p;
    QPoint mouseFits = mouse2fitsi( lastKnownMousePosition());

    if( ! isCursorFrozen ())
        m_lastCursorFitsPosition = mouseFits;

    cursorChangedUpdate ( );
}

// callback for mouse moves in the canvas
void
FvController::canvasMouseMoveCB (
        QMouseEvent * event)
{
    setLastKnownMousePosition( event->posF ());
}

// callback for mouse clicks in the canvas
void
FvController::canvasMousePressCB (
        QMouseEvent * /*event*/)
{
    /*

    //    s_dataloaded = true;
    QPointF p( event->x(), event->y());

//    dbg(1) << "click  " << p.x() << " " << p.y() << "\n";
//    QPointF pp = mouse2imagef(p);
//    dbg(1) << "clicki " << pp.x() << " " << pp.y() << "\n";
//    QPointF ppp = image2mousef( pp);
//    dbg(1) << "clickf " << ppp.x() << " " << ppp.y() << "\n";
    centerScreenPoint ( p);
    return;

    setLastKnownMousePosition( event->posF ());

    if( event-> button () == Qt::LeftButton) {
        canvasLastClickPosition_ = event-> posF ();
        dragDrawOrigin_ = drawOrigin();
        dragDrawWidth_ = drawWidth();
    }

    */
}

// callback for mouse releases in the canvas
void
FvController::canvasMouseReleaseCB (
        QMouseEvent * event)
{
    setLastKnownMousePosition( event->posF ());
}

// callback for mouse-wheel events in the canvas
void
FvController::canvasMouseWheelCB (
        QWheelEvent * event)
{
    dbg(1) << "qt wheel event delta=" << event->delta() << "\n";

    double delta = - event->delta();

    // remember the image point and where on the screen it was
    QPointF screenOrig = lastKnownMousePosition();
    // remember which image point the mouse was pointing to
    QPointF imgOrig = mouse2imagef( screenOrig);

    // apply zoom
    int inc = 10;
    int sz = drawWidth2sliderZoom ( drawWidth ());
    if( delta < 0) sz -= inc; else sz += inc;
    sz = clamp( sz, 0, 1000);
    setDrawWidth ( sliderZoom2drawWidth (sz));

    // make sure that the image original image point maps
    // to the same screen point...
    QPointF screenNew = image2mousef( imgOrig);
    setDrawOrigin( drawOrigin() + screenOrig - screenNew);
}

// key-event
void
FvController::canvasKeyPressCB (QKeyEvent * /*event*/)
{
    //    dbg(1) << "QKeyEvent: " << event-> key () << " : " << event->modifiers ()
    //            << " : " << event->text ()
    //            << "\n";

}

// schedules a repaint (unless there is already one pending)
void
FvController::canvasRepaint ()
{
    //    dbg(1) << "canvasRepaint\n";
    m_isRepaintNeeded = true;

    // tell pureweb to render
    GetStateManager().ViewManager().RenderViewDeferred( m_pwViewName.toStdString());

//    if( repaintCheckQueued_) return;
//    repaintCheckQueued_ = true;
//    qApp-> postEvent ( this, new QEvent( static_cast<QEvent::Type> (repaintEventType_)));
}

// redraws the frame
void
FvController::canvasRepaintRaw ()
{
    if( ! m_isRepaintNeeded) {
        return;
    }

    ScopedDebug sd( "canvasRepaintRaw", 1);
    m_isRepaintNeeded = false;

    // render FITS into image
    QImage img;
    if( m_isFileLoaded) {
        img = m_fvs.renderCurrentFrame ();

//        // remember the last drawn rectangle
//        lastVisibleRect_ = QRectF(
//                    mouse2imagef (QPointF(0,0)),
//                    mouse2imagef (QPointF(img.width (), img.height ())));
    } else {
        // default image before we load anything
        img = QImage( m_fvs.size (), QImage::Format_RGB888);
        QPainter p( & img);
        p.setRenderHint ( QPainter::TextAntialiasing, true);
        p.setPen( QPen( QColor( 0, 128, 0), 2));
        p.setFont ( QFont( "Helvetica", 24));
        p.fillRect ( img.rect (), "black");
        p.drawText ( img.rect (), Qt::AlignCenter, "Loading...");
    }

    // render gaussians
    if( g2dResults_.params.size() > 0) {
        QPainter p( & img);
        p.setRenderHint( p.Antialiasing);
        const std::vector<double> & x = g2dResults_.params;
        int ng = x.size() / 6;
        p.setBrush( QColor( 255, 255, 255, 64 ));
        p.setPen( QPen( QColor("green"), 3));
        QColor brc( 0, 0, 0, 127);
        QColor fc( "white");
        for( int i = 0 ; i < ng ; i ++ ) {
            //            double ampl   = x[i*6 + 0];
            double xc     = x[i*6 + 1];
            double yc     = x[i*6 + 2];
            double width  = x[i*6 + 3];
            double height = x[i*6 + 4] * width;
            double angle  = x[i*6 + 5];
            xc += 1.5;
            yc += 0.5;
            width /= 2.0;
            height /= 2.0;
            QPointF center = fits2mousef( QPointF( xc, yc));
            //            QPointF size0 = fits2mousef( QPointF( 0, 0));
            //            QPointF size1 = fits2mousef( QPointF( width, height));
            //            QPointF size = size1 - size0;
            QPointF corner = fits2mousef( QPointF( xc+width, yc+height));
            QPointF size = corner - center;
            //            xc = img.width() / 2; yc = img.height() / 2;
            //            width = img.width() / 4;
            //            height = width / 1.5;
            //            angle = i * 15;
            p.save();
            p.translate( center);
            p.rotate( 90 - angle);
            p.drawLine( QPointF(-size.x(),0), QPointF(size.x(),0));
            p.drawLine( QPointF(0,-size.y()), QPointF(0,size.y()));
            p.drawEllipse( QPointF(0.0, 0.0), size.x(), size.y());
            p.restore();
            if( i > 0) {
                p.save();
                QRectF br = p.boundingRect(QRectF(center.x(),center.y(),1,1),
                                           Qt::AlignCenter | Qt::TextDontClip,
                                           QString("#%1").arg(i+1));
                p.fillRect( br, brc);
                p.setPen( fc);
                p.drawText(QRectF(center.x(),center.y(),1,1),
                           Qt::AlignCenter | Qt::TextDontClip,
                           QString("#%1").arg(i+1));
                p.restore();
            }
        }
    }

    if( m_gridVisible && m_isFileLoaded) {
        SkyGridPlotter gp;
        gp.setFitsHeader(m_fvs.getHeaderInfo().headerLines.join(""));
        gp.setOutputImage( & img);
        int marginTop = 5;
        int marginLeft = 40;
        int marginRight = 5;
        int marginBottom = 40;
        QRectF orect = QRectF(
                    marginLeft,
                    marginTop,
                    img.width() - marginLeft - marginRight,
                    img.height() - marginTop - marginBottom);
        // erase margins
        {
            QColor blank( 0, 0, 0, 255);
            QPainter p( & img);
            p.fillRect( 0, 0, img.width(), marginTop, blank);
            p.fillRect( 0, marginTop, marginLeft, img.height() - marginTop - marginBottom, blank);
            p.fillRect( img.width() - marginRight, marginTop, marginRight, img.height() - marginTop - marginBottom, blank);
            p.fillRect( 0, img.height() - marginBottom, img.width(), marginBottom, blank);

        }
        gp.setOutputRect( orect);
        QRectF irect = QRectF(
                    mouse2fitsf (orect.topLeft()),
                    mouse2fitsf (orect.bottomRight()));

        gp.setInputRect( irect);
        gp.setPlotOption( "tol=0.001");
        //    gplot.setPlotOption( "Colour(grid)=2, Font(textlab)=3, tol=0.001" );
        //    gplot.setPlotOption( "Digits=8" );
        gp.setPlotOption( "Labelling=Exterior");
        //    gplot.setPlotOption( "DrawAxes=0");
        gp.setPlotOption( "DrawTitle=0");
        //    gplot.setPlotOption( "TitleGap=0.005");
        //    gplot.setPlotOption( "Edge(2)=left");
        gp.setPlotOption( "Width(axes)=2");
        gp.setPlotOption( "Width(border)=2");
        gp.setPlotOption( "Width(Grid)=0.5");
        //    gplot.setPlotOption( "Format(1)=gd");
        //    gplot.setPlotOption( "Format(2)=gd");
        //    gplot.setPlotOption( "LabelUnits=1");
        gp.setPlotOption( "LabelUp(2)=0");
        gp.setPlotOption( "Size=9" );
        gp.setPlotOption( "TextLab(1)=1" );
        gp.setPlotOption( "TextLab(2)=1" );
        gp.setPlotOption( "Size(TextLab1)=11" );
        gp.setPlotOption( "Size(TextLab2)=11" );
        //    gplot.setPlotOption( "NumLabGap=0.001");
        //    gplot.setPlotOption( "TextLabGap=0.001");

        //        if( qrand() % 2) {
        //            gp.setPlotOption( "System=Galactic");
        //        } else {
        //            gp.setPlotOption( "System=J2000");
        //        }

        dbg(1) << "Drawing grid in " << m_fvs.getWCS() << "\n";

        gp.setSystem( m_fvs.getWCS());

//        if( m_fvs.getWCS() == "J2000") {
//            gp.setSystem( SkyGridPlotter::FK5);
//        } else if( m_fvs.getWCS() == "B1950") {
//            gp.setSystem( SkyGridPlotter::FK4);
//        } else if( m_fvs.getWCS() == "ICRS") {
//            gp.setSystem( SkyGridPlotter::ICRS);
//        } else if( m_fvs.getWCS() == "GALACTIC") {
//            gp.setSystem( SkyGridPlotter::Galactic);
//        } else if( m_fvs.getWCS() == "ECLIPTIC") {
//            gp.setSystem( SkyGridPlotter::Ecliptic);
//        } else {
//            gp.setSystem( SkyGridPlotter::Default);
//        }

        if( ! gp.plot()) {
            dbg(1) << "could not plot grid: " + gp.getError() << "\n";
        }
    }

    // the buffer needs rgb888 format
    m_buffer = img.convertToFormat ( QImage::Format_RGB888);

//    m_gui-> ui ()-> canvasWidget-> setCanvas ( img);

    // the cursor could be potentially over a different pixel now, so recalculate it
    setLastKnownMousePosition ( lastKnownMousePosition ());

    // send the visible rectangle to clients
    sendVisibleRectangle();
}


// slot for frame requests
void
FvController::setFrame (
        int frame)
{
    dbg(1) << "setFrame " << frame << "\n";

    m_currentFrame = frame;
    m_fvs.loadFrame ( m_currentFrame);

    dbg(1) << "frame loaded\n";

    // tell services about frame change
    rsInput_.currentFrame = frame;
    m_rsManager->request( rsInput_);
    m_g2dInput.currentFrame = frame;
    m_g2dfitManager->request( m_g2dInput);

    m_gui->movieDialog_->frameChanged( m_currentFrame);

    // update histogram dialog with current values
    m_gui-> histogramDialog_->setFrameInfo (
                m_fvs.currentFrame (),
                m_fvs.cir());
    // update the status
//    cursorChangedUpdate ();

    canvasRepaint ();

}

// requests a frame to be loaded
void
FvController::requestFrame ( int frame)
{
    // we delay the frame request by the smallest amount of time, in case we receive
    // a lot of them in a row...
    requestedFrame_ = frame;
    requestedFrameTimer_.start ( 1);
}

void
FvController::requestedFrameTimerCB ()
{
    if( requestedFrame_ != m_currentFrame) {
        setFrame ( requestedFrame_);
    }
    requestedFrameTimer_.stop ();
}

void
FvController::histogramCB(
        double min,
        double max)
{
    setHistogram( min, max);
    canvasRepaint ();
}

// we receive this event when client has acknowledged rendering some version of the view
void FvController::onViewUpdated(const ValueChangedEventArgs & args)
{
    qint64 val = args.NewValue().ConvertOr<qint64>(-1);
    dbg(1) << "last id/curr id = " << m_lastRepaintId << "/" << val << "\n";
    if( m_lastRepaintId == val) {
        // we are in sync...
        emit mainViewIsSynchronized();
    }

}

void
FvController::setHistogram (
        double min,
        double max)
{
    m_histMin = min;
    m_histMax = max;
    m_fvs.setHistogram (m_histMin, m_histMax);
}

// callback for colormaps
void
FvController::colormapCB(
        ColormapFunction fn)
{
    setColormap( fn);
    canvasRepaint ();
}

// set the current colormap & repaint
void
FvController::setColormap(
        const ColormapFunction & fn)
{
    cmap_ = fn;
    m_fvs.setColormap ( cmap_);
}

void
FvController::loadFileShortcutCB(CSI::Guid /*sessionid*/, CSI::Typeless command, CSI::Typeless /*responses*/)
{
    int ind = command["/index"].ValueOr<int>(-1);
    dbg(1) << "load shortcut index = " << ind << "\n";
    if( ind < 0 || ind >= int( m_fileShortcuts.size())) return;

    QString fname = m_fileShortcuts[ind].c_str();
    setTitle( QFileInfo(fname).baseName());
    FitsFileLocation floc = FitsFileLocation::fromLocal( fname);
    loadFileAuto( floc);
}

// slot for loading a new file
void
FvController::loadMainFile (
        const FitsFileLocation & pfloc)
{
    FitsFileLocation floc = pfloc;

    dbg(1) << "loadFile " << floc.getLocalFname () << "\n";
    m_gui-> statusBar ()-> showMessage ( "Loading");

    // erase region box
    rsInput_.isNull = true;
    m_rsManager->request( rsInput_);

    if( ! m_fvs.loadFitsFile ( floc)) {
        LTHROW("Could not load fits file");
        //        floc = m_defaultImageLocation;
        //        if( ! fvs_.loadFitsFile ( floc)) return;
    }
    m_fLocation = floc;
    m_isFileLoaded = true;

    pwsetdc( "image_dims", QString("%1 %2 %3")
             .arg(m_fvs.getHeaderInfo().naxis1)
             .arg(m_fvs.getHeaderInfo().naxis2)
             .arg(m_fvs.getHeaderInfo().totalFrames));

    // update the UI a bit
    m_gui-> movieDialog_-> setNFrames ( m_fvs.getHeaderInfo ().totalFrames);

    // figure out centered parameters dw, dorig
    double dw;
    QPointF dorig;
    m_fvs.getCentered ( dorig, dw);
    // set these values
    setDrawWidth ( dw);
    setDrawOrigin ( dorig);
    m_fvs.resize ( m_gui->ui ()->canvasWidget->size ());

    setFrame (0);
    zoomSliderCB ( 0);
    m_gui-> histogramDialog_-> activatePreset ( 4, false); // 99.5%
    m_gui-> histogramDialog_-> showFullRange();
    //    canvasRepaint ();

    // update the header dialog
    m_gui-> fitsHeaderDialog_-> setHeaderLines(
                m_fvs.getHeaderInfo ().headerLines);

    m_gui-> statusBar ()-> showMessage ( "Ready");
}

void
FvController::loadSingleImage( const FitsFileLocation & floc)
{
    loadMainFile ( floc);

    // set the locations for q/u profiles to nothing...
    FitsFileLocation qloc, uloc;
    m_profileController-> setLocations (
                m_fvs.cir()->cc ().getInitParameters (), floc,
                qloc, uloc);
    m_profileController-> updateWcs( m_fvs.wcsHero());

//    GetStateManager().XmlStateManager().SetValueAs<bool>(
//                "/isIQUcube", false);
//    pwset("/isIQUcube", false);
}


void
FvController::loadIQUset(const FitsFileLocation &floc)
{
    QString fname = floc.getLocalFname();

    dbg(1) << "loadCube " << fname << "\n";

    QSettings qs( fname, QSettings::IniFormat);
    bool ok;
    int version = qs.value ("version", -1).toInt ( & ok);
    if( ! ok) version = -1;
    if( version != 1) {
        dbg(1) << "Unknown version in " << fname << "\n";
        LTHROW( "Bad version in IQU file");
    }
    QString cubeIfname = qs.value ( "cubeI").toString ();
    QString cubeQfname = qs.value ( "cubeQ").toString ();
    QString cubeUfname = qs.value ( "cubeU").toString ();
    QString cubeVfname = qs.value ( "cubeV").toString ();
    QString description = qs.value( "description", "No description").toString ();

    dbg(1) << "cubes:\n"
           << "   - " << cubeIfname << "\n"
           << "   - " << cubeQfname << "\n"
           << "   - " << cubeUfname << "\n"
           << "   - " << cubeVfname << "\n";
    dbg(1) << "Description:\n"
           << description << "\n";

    if( cubeIfname.isEmpty () || cubeQfname.isEmpty () || cubeUfname.isEmpty ()) {
        dbg(1) << "I, Q and U cubes must all be specified.\n";
        LTHROW("I, Q and U must be specified, one of them is not");
        return;
    }

    // resolve relative/absolute filenames with respect to the directory of
    // the ini file
    QString prefix = QFileInfo( fname).dir ().path ();
    if( prefix == "/") prefix = "";
    dbg(1) << "Prefix: " << prefix << "\n";
    if( ! cubeIfname.startsWith ( '/')) cubeIfname = prefix + "/" + cubeIfname;
    if( ! cubeQfname.startsWith ( '/')) cubeQfname = prefix + "/" + cubeQfname;
    if( ! cubeUfname.startsWith ( '/')) cubeUfname = prefix + "/" + cubeUfname;
    if( ! cubeVfname.startsWith ( '/')) cubeVfname = prefix + "/" + cubeVfname;

    dbg(1) << "absolute cubes:\n"
           << "   - " << cubeIfname << "\n"
           << "   - " << cubeQfname << "\n"
           << "   - " << cubeUfname << "\n"
           << "   - " << cubeVfname << "\n";

    // convert to cannonical paths
    cubeIfname = QFileInfo( cubeIfname).canonicalFilePath ();
    cubeQfname = QFileInfo( cubeQfname).canonicalFilePath ();
    cubeUfname = QFileInfo( cubeUfname).canonicalFilePath ();
    cubeVfname = QFileInfo( cubeVfname).canonicalFilePath ();

    dbg(1) << "canonical cubes:\n"
           << "   - " << cubeIfname << "\n"
           << "   - " << cubeQfname << "\n"
           << "   - " << cubeUfname << "\n"
           << "   - " << cubeVfname << "\n";

    if( cubeIfname.isEmpty () || cubeQfname.isEmpty () || cubeUfname.isEmpty ()) {
        dbg(1) << "I, Q and U cubes must all be specified.\n";
        LTHROW( "One of I, Q or U cubes does not exist.");
        return;
    }

    FitsFileLocation iloc = FitsFileLocation::fromLocal ( cubeIfname);
    FitsFileLocation qloc = FitsFileLocation::fromLocal ( cubeQfname);
    FitsFileLocation uloc = FitsFileLocation::fromLocal ( cubeUfname);

    loadMainFile ( iloc);
    m_profileController-> setLocations (
                m_fvs.cir()->cc ().getInitParameters (),
                iloc, qloc, uloc);
    m_profileController-> updateWcs( m_fvs.wcsHero());

    // queue up Q and U cube for cache generator
    m_fvs.queueForCacher ( qloc);
    m_fvs.queueForCacher ( uloc);
}

void FvController::loadFileAuto(const FitsFileLocation &floc)
{
    try {

        // if the extensions is '.ini' or '.iqu' then try to load it as IQU set
        QString fname = floc.getLocalFname();
        if( fname.endsWith (".ini", Qt::CaseInsensitive)
                || fname.endsWith (".iqu", Qt::CaseInsensitive))
        {
            loadIQUset ( floc);
            setCursorFrozen( false);
            pwset( "/loadedImageType", "iqu-fake");
            return;
        }

        // if it is a special FITS file that only WE understand, i.e. it has
        // the IQUSET keyword set, then it's the ELGG hack to load
        // an IQU set...
        FitsParser p;
        if( p.loadFile( floc)) {
            int ind = p.getHeaderInfo().iquset;
            if( ind != -1) {
                if( m_elggHacks.find( ind) != m_elggHacks.end()) {
                    loadIQUset( FitsFileLocation::fromLocal( m_elggHacks[ind].c_str()));
                    setCursorFrozen( false);
                    pwset( "/loadedImageType", "iqu-fake");
                    return;
                } else {
                    LTHROW( "Invalid IQU set in fake FITS file");
                }
            }
        }

        // otherwise we'll try to load it as a regular fits file
        loadSingleImage ( floc);
        setCursorFrozen( false);
        // based on naxis keywords figure out the type
        QString type;
        if( m_fvs.getHeaderInfo().naxis == 2) {
            type = "xy";
        }
        if( type.isEmpty() && m_fvs.getHeaderInfo().totalFrames == 1) {
            type = "xy1";
        }
        if( type.isEmpty()) {
            type = "xyz";
        }
        pwset( "/loadedImageType", type);
        return;
    } catch ( ... ) {

    }

    // if we get here, it means we could not load the requested file
    // so we try to load the 'error' file
    dbg(1) << "Failed to load file " << floc.toStr() << "\n";
    dbg(1) << "Trying to load the error image.\n";
    loadSingleImage( m_defaultImageLocation);
    setCursorFrozen( false);
    pwset( "/loadedImageType", "error");
}

void FvController::fileBrowserLoadCB(QString fname)
{
    setTitle( QFileInfo(fname).baseName());
    loadFileAuto( FitsFileLocation::fromLocal( fname));
}

// convert drawWidth to pixel zoom and vice versa
double
FvController::drawWidth2pixelZoom( double dw)
{
    double iw = m_fvs.getHeaderInfo ().naxis1;
    double ww = m_gui->ui ()->canvasWidget->rect ().width ();
    return dw * ww / iw;
}

double
FvController::pixelZoom2drawWidth( double pzoom)
{
    double iw = m_fvs.getHeaderInfo ().naxis1;
    double ww = m_gui->ui ()->canvasWidget->rect ().width ();
    //    double ih = fvs_.getHeaderInfo ().naxis2;
    //    double wh = fvView_->ui ()->canvasWidget->rect ().height ();

    return pzoom * iw / ww;
}

int
FvController::drawWidth2sliderZoom (double dw)
{
    double iw = m_fvs.getHeaderInfo ().naxis1;
    double ww = m_gui->ui ()->canvasWidget->rect ().width ();
    double ih = m_fvs.getHeaderInfo ().naxis2;
    double wh = m_gui->ui ()->canvasWidget->rect ().height ();

    double minZoom = std::min( ww/iw * 0.9, wh/ih * 0.9);
    double maxZoom = 64;

    double pzoom = drawWidth2pixelZoom ( dw);
    double x = (pzoom - minZoom) / (maxZoom - minZoom);
    x = clamp( x, 0.0, 1.0);
    x = sqrt(x);
    x = x * (exp(1) - 1) + 1;
    x = log(x);
    x = x * 1000;
    if( x < 0) x = 0; if( x > 1000) x = 1000;
    int izoom = round(x);

    return izoom;
}

double
FvController::sliderZoom2drawWidth (int szoom)
{

    double iw = m_fvs.getHeaderInfo ().naxis1;
    double ih = m_fvs.getHeaderInfo ().naxis2;

    // clamp zoom to 0-1000 range
    szoom = clamp( szoom, 0, 1000);

    double ww = m_gui->ui ()->canvasWidget->rect ().width ();
    double wh = m_gui->ui ()->canvasWidget->rect ().height ();

    double zoom = (exp( szoom / 1000.0) - 1) / (exp(1.0) - 1);
    zoom *= zoom;
    double minZoom = std::min( ww/iw * 0.9, wh/ih * 0.9);
    double maxZoom = 64;
    zoom = (1-zoom) * minZoom + zoom * maxZoom;

    return pixelZoom2drawWidth ( zoom);
}


// this is the only function controlling zoom via drawWidth mechanism
void
FvController::setDrawWidth (double v)
{
    double iw = m_fvs.getHeaderInfo ().naxis1;
    double ww = m_gui->ui ()->canvasWidget->rect ().width ();
    double ih = m_fvs.getHeaderInfo ().naxis2;
    double wh = m_gui->ui ()->canvasWidget->rect ().height ();

    double minPixelZoom = std::min( ww/iw * 0.9, wh/ih * 0.9);
    double maxPixelZoom = 64;

    double v2 = pixelZoom2drawWidth ( clamp( drawWidth2pixelZoom ( v), minPixelZoom, maxPixelZoom));

    m_drawWidth = v2;
    m_fvs.setDrawWidth ( m_drawWidth);
    canvasRepaint ();

    // update pureweb state
    double pz = drawWidth2pixelZoom ( m_drawWidth);
    QString zoomString = QString("%1").arg( pz * 100.0, 7, 'f', 2);
    pwset( "/PixelZoom", zoomString);

    int sz = round( drawWidth2sliderZoom ( m_drawWidth));
//    GetStateManager().XmlStateManager().SetValueAs<int>("/SliderZoom", sz);
    pwset("/SliderZoom", sz);
}

// this is the only function for setting draw origin
void
FvController::setDrawOrigin (const QPointF & p)
{
    m_drawOrigin = p;
    minimizeBlankSpace ();
    m_fvs.setDrawOrigin ( m_drawOrigin);
    canvasRepaint ();
}

// convenience function of the above
void
FvController::setDrawOrigin (qreal x, qreal y)
{
    setDrawOrigin ( QPointF( x, y));
}

// minimize blank space around the image
// this is an internal function, which only updates drawOrigin_ variable
void
FvController::minimizeBlankSpace ()
{
    dbg(1) << "Minimizing blank space...\n";
    double maxHspaceLeft = 10,
            maxHspaceRight = 10,
            maxVspaceTop = 10,
            maxVspaceBottom = 62;

    if( m_gridVisible) {
        maxHspaceLeft += 40;
        maxHspaceRight += 5;
        maxVspaceTop += 5;
        maxVspaceBottom = 40;
    }

    // horizontal fix first:
    QPointF tl = image2mousef ( QPointF(0,0));
    QPointF br = image2mousef ( QPointF( m_fvs.getHeaderInfo ().naxis1,
                                         m_fvs.getHeaderInfo ().naxis2));

    {
        double x1 = tl.x (), x2 = br.x ();
        double ww = m_fvs.size().width();
        double iw = br.x() - tl.x();
        double left = x1;
        double right = ww - x2;
        // special case - if the entire image can fit within constraints
        if( br.x() - tl.x() < ww - maxHspaceLeft - maxHspaceRight) {
            x1 = maxHspaceLeft + (ww - maxHspaceLeft - maxHspaceRight - br.x() + tl.x())/2;
        } else {
            if( left <= maxHspaceLeft) {
                // left is ok
                if( right <= maxHspaceRight) {
                    // right is ok as well, do nothing
                } else {
                    // right is too big, fix it by moving it to aligh with max value
                    x1 = ww - maxHspaceRight - iw;
                }
            } else {
                // left needs fixing
                if( right <= maxHspaceRight) {
                    // right does not need fixing, so only fix left
                    x1 = maxHspaceLeft;
                } else {
                    // both need fixing, in this case center it within the margins
                    x1 = maxHspaceLeft + (ww - maxHspaceLeft - maxHspaceRight - iw) / 2.0;
                }
            }

        }

        m_drawOrigin.setX ( x1);
    }

    {
        double y1 = tl.y (), y2 = br.y ();
        double wh = m_fvs.size().height();
        double ih = br.y() - tl.y();
        double top = y1;
        double bottom = wh - y2;
        // special case - if the entire image can fit within constraints
        if( br.y() - tl.y() < wh - maxVspaceTop - maxVspaceBottom) {
            y1 = maxVspaceTop + (wh - maxVspaceTop - maxVspaceBottom - br.y() + tl.y())/2;
        } else {
            if( top <= maxVspaceTop) {
                // top is ok
                if( bottom <= maxVspaceBottom) {
                    // bottom is ok as well, do nothing
                } else {
                    // bottom is too big, fix it by moving it to aligh with max value
                    y1 = wh - maxVspaceBottom - ih;
                }
            } else {
                // top needs fixing
                if( bottom <= maxVspaceBottom) {
                    // bottom does not need fixing, so only fix top
                    y1 = maxVspaceTop;
                } else {
                    // both need fixing, in this case center it within the margins
                    y1 = maxVspaceTop + (wh - maxVspaceTop - maxVspaceBottom - ih) / 2.0;
                }
            }
        }

        m_drawOrigin.setY ( y1);
    }


}

/*
void
FvController::minimizeBlankSpace ()
{
    double maxHspaceLeft = 10,
            maxHspaceRight = 10,
            maxVspaceTop = 10,
            maxVspaceBottom = 10;

    if( gridVisible_) {
        maxHspaceLeft += 40;
        maxHspaceRight += 5;
        maxVspaceTop += 5;
        maxVspaceBottom += 40;
    }

    // horizontal fix first:
    QPointF tl = image2mousef ( QPointF(0,0));
    QPointF br = image2mousef ( QPointF( fvs_.getHeaderInfo ().naxis1,
                                         fvs_.getHeaderInfo ().naxis2));
    double x1 = tl.x (), x2 = br.x ();
    if( x1 > maxHspaceLeft) {
        // too much space on left
        double ox = maxHspaceLeft - x1;
        x1 += ox;
        x2 += ox;
    }
    if( fvs_.size ().width () - x2 > maxHspaceRight) {
        // too much space on right
        double ox = fvs_.size ().width () - maxHspaceRight - x2;
        x1 += ox;
        x2 += ox;
    }
    if( x1 >= maxHspaceLeft && fvs_.size ().width () - x2 >= maxHspaceRight) {
        // too much space on both sides
        double ox = (fvs_.size ().width () - x2 - x1) / 2.0;
        x1 += ox;
        x2 += ox;
    }
    drawOrigin_.setX ( drawOrigin_.x () + x1 - tl.x ());

    double y1 = tl.y (), y2 = br.y ();
    if( y1 > maxVspaceTop) {
        // too much space on top
        double oy = maxVspaceTop - y1;
        y1 += oy;
        y2 += oy;
    }
    if( fvs_.size ().height () - y2 > maxVspaceBottom) {
        // too much space on bottom
        double oy = fvs_.size ().height () - maxVspaceBottom - y2;
        y1 += oy;
        y2 += oy;
    }
    if( y1 >= maxVspaceTop && fvs_.size ().height () - y2 >= maxVspaceBottom) {
        // too much space on both sides
        double oy = (fvs_.size ().height () - y2 - y1) / 2.0;
        y1 += oy;
        y2 += oy;
    }
    drawOrigin_.setY ( drawOrigin_.y () + y1 - tl.y ());

}

 */

// convert mouse coordinates to FITS, with double precision
QPointF
FvController::mouse2fitsf(
        const QPointF & p)
{
    // just use mouse-> image -> fits
    return image2fitsf ( mouse2imagef (p));
}

// convert FITS coordinates to mouse (screen) coordinstes, with double precision
QPointF
FvController::fits2mousef (const QPointF & p)
{
    // first convert to image coordinates
    QPointF pim = fits2imagef( p);
    // then from image to mouse
    return image2mousef( pim);
}

// convert mouse coordinates to FITS, rounding to integer coordinates
// this is essentially a convenience wrapper
QPoint
FvController::mouse2fitsi(
        const QPointF & p)
{
    QPointF pf = mouse2fitsf (p);
    return QPoint( floor (pf.x ()), floor (pf.y ()));
}

// convert mouse coordinates to image coordinates, double precision
QPointF
FvController::mouse2imagef (
        const QPointF &p)
{
//    dbg(1) << "m2i dw="<<drawWidth()<<" ww="<<m_gui->ui ()->canvasWidget->width ()<<" o="<<drawOrigin()<<" p="<<p;
    double pw = drawWidth() / m_fvs.getHeaderInfo ().naxis1 * m_gui->ui ()->canvasWidget->width ();
    double ax = 1.0 / pw;
    double ay = ax; // same
    double bx = - ax * drawOrigin().x ();
    double by = - ay * drawOrigin().y ();

    //    dbg(1) << " m2if " << ax * p.x () + bx << " " <<  ay * p.y () + by << "\n";

    return QPointF( ax * p.x () + bx, ay * p.y () + by);
}

QPointF
FvController::image2mousef(const QPointF & p)
{
    double x1 = drawOrigin().x ();
    double y1 = drawOrigin().y ();
    double iw = m_fvs.getHeaderInfo ().naxis1;
    double ww = m_gui->ui ()->canvasWidget->width ();
    double ih = m_fvs.getHeaderInfo ().naxis2;
    //    double wh = fvView_->ui ()->canvasWidget->height ();
    double w = ww * drawWidth ();
    double h = (ih/iw) * w;
    double x2 = x1 + w;
    double y2 = y1 + h;
    double xr = p.x () / iw;
    double yr = p.y () / ih;
    double x = (1-xr) * x1 + xr * x2;
    double y = (1-yr) * y1 + yr * y2;
    return QPointF( x, y);
}

// convert mouse coordinates to image coordinates, integer rounded
QPoint
FvController::mouse2imagei (
        const QPointF &p)
{
    QPoint pi = mouse2fitsi( p);
    return QPoint( pi.x()-1, m_fvs.getHeaderInfo ().naxis2 - pi.y ());

    //    QPointF pf = mouse2imagef ( p);
    //    return QPoint( round(pf.x ()), round(pf.y ()));
}

// convert image to fits coordinates, double precision
QPointF
FvController::image2fitsf (
        const QPointF &p)
{
    return QPointF( p.x () + 1, m_fvs.getHeaderInfo ().naxis2 - p.y () + 1);
}
QPoint
FvController::image2fitsi (const QPointF &p)
{
    QPointF pf = image2fitsf ( p);
    return QPoint( pf.x (), pf.y ());
}
QPointF
FvController::fits2imagef (const QPointF & p)
{
    return QPointF( p.x () - 1, m_fvs.getHeaderInfo ().naxis2 - p.y ());
}

void FvController::RenderView(CSI::PureWeb::Server::RenderTarget image)
{
    ScopedDebug sd( "RenderView", 1);

    // repaint if needed
    canvasRepaintRaw();

//    dbg(1) << "in renderview buff = " << buffer_.width() << " x " << buffer_.height() << "\n";

    if( GlobalSettings::isDebug()) {
        QPainter p( & m_buffer);
        p.setRenderHint ( QPainter::TextAntialiasing, true);
        p.setPen( QPen( QColor( 0, 128, 0), 2));
        p.setFont ( QFont( "Helvetica", std::min( m_buffer.width(), m_buffer.height()) / 20));
        QString txt = QString( "Frame %1").arg( m_currentFrame + 1);
        double tx = m_buffer.width()/2.0;
        double ty = m_buffer.height()/2.0;
        tx = 10; ty = 10;
        QRectF br = p.boundingRect(
                    QRectF(tx, ty, 1, 1),
                    Qt::AlignTop | Qt::AlignLeft | Qt::TextDontClip,
                    txt);
        p.fillRect ( br, QColor(0,0,0,40));
        p.setOpacity( 0.4);
        p.setPen( QColor( "yellow"));
        p.drawText (
                    QRectF(tx, ty, 1, 1),
                    Qt::AlignTop | Qt::AlignLeft | Qt::TextDontClip,
                    txt);
    }

    // copy the buffer into PureWeb storage so that it can compress it and send it to
    // the client
    ByteArray bits = image.RenderTargetImage().ImageBytes();
    ByteArray::Copy(m_buffer.scanLine(0), bits, 0, bits.Count());

    // attach extra info to the frame
    // ==============================
    CSI::Collections::Map<String,String> map = image.Parameters();

    // attach trasnformation information
    double txa = 1, txb = 0, tya = 1, tyb = 0;
    if( m_isFileLoaded) {
        // coordinate transformation
        QPointF p1 = image2mousef( QPoint(0,0));
        QPointF p2 = image2mousef( QPoint(1,1));
        txa = p2.x() - p1.x();
        txb = p1.x();
        tya = p2.y() - p1.y();
        tyb = p1.y();
    }
    map["txa"] = QString::number(txa).toStdString();
    map["txb"] = QString::number(txb).toStdString();
    map["tya"] = QString::number(tya).toStdString();
    map["tyb"] = QString::number(tyb).toStdString();
    m_lastRepaintId ++;
    map["updateID"] = QString::number( m_lastRepaintId).toStdString();

//    QPointF newCenterPt = mouse2imagef( QPointF( m_fvs.size().width() / 2.0, m_fvs.size().height() / 2.0));
//    dbg(1) << "centerPt = " << newCenterPt;
}

void FvController::PostMouseEvent(const CSI::PureWeb::Ui::PureWebMouseEventArgs& mouseEvent)
{
    //    std::string s = mouseEvent.ViewName ().As<std::string>();
    //    QString xxx = QString("Mouse event from view %1").arg(s.c_str ());
    //    fvView_->ui ()->debugWidget->append (xxx);

    Qt::MouseButtons buttons = 0;
    Qt::KeyboardModifiers keys = 0;
    QEvent::Type type = QEvent::None;
    Qt::MouseButton initate = Qt::NoButton;

    //    fvView_-> ui ()->debugWidget->append ( QString("Mouse at %1 %2").arg(mouseEvent.X).arg(mouseEvent.Y));

    if (0 != (mouseEvent.Modifiers & CSI::PureWeb::Ui::Modifiers::Shift))     keys |= Qt::ShiftModifier;
    if (0 != (mouseEvent.Modifiers & CSI::PureWeb::Ui::Modifiers::Control))   keys |= Qt::ControlModifier;
    if (0 != (mouseEvent.Buttons & CSI::PureWeb::Ui::MouseButtons::Left))     buttons |= Qt::LeftButton;
    if (0 != (mouseEvent.Buttons & CSI::PureWeb::Ui::MouseButtons::Right))    buttons |= Qt::RightButton;
    if (0 != (mouseEvent.Buttons & CSI::PureWeb::Ui::MouseButtons::Middle))   buttons |= Qt::MidButton;
    if (0 != (mouseEvent.Buttons & CSI::PureWeb::Ui::MouseButtons::XButton1)) buttons |= Qt::XButton1;
    if (0 != (mouseEvent.Buttons & CSI::PureWeb::Ui::MouseButtons::XButton2)) buttons |= Qt::XButton2;

    CSI::PureWeb::Ui::MouseEventType::Enum mouseEventType = mouseEvent.EventType;
    switch(mouseEventType)
    {
    case CSI::PureWeb::Ui::MouseEventType::MouseWheel:
    {
        dbg(1) << "PostMouseEvent - wheel, delta = " << mouseEvent.Delta << "\n";
        int delta = mouseEvent.Delta;
        if( mouseEvent.Delta > 0 && mouseEvent.Delta < 1) delta = 1;
        if( mouseEvent.Delta < 0 && mouseEvent.Delta > -1) delta = -1;
        QWheelEvent * wheel = new QWheelEvent(QPoint(mouseEvent.X,mouseEvent.Y), delta, buttons, keys);
        QCoreApplication::postEvent(m_gui->ui ()->canvasWidget, wheel);
        return ;
    }
    case CSI::PureWeb::Ui::MouseEventType::MouseEnter:
    case CSI::PureWeb::Ui::MouseEventType::MouseLeave:
    {
        return;
    }
    case CSI::PureWeb::Ui::MouseEventType::MouseDown:
    {
        type = QMouseEvent::MouseButtonPress;
        initate = Qt::LeftButton;
        break;
    }
    case CSI::PureWeb::Ui::MouseEventType::MouseUp:
    {
        type = QMouseEvent::MouseButtonRelease;
        initate = Qt::LeftButton;
        break;
    }
    case CSI::PureWeb::Ui::MouseEventType::MouseMove:
    {
        type = QMouseEvent::MouseMove;
        initate = Qt::NoButton;
        break;
    }
    default: {}
    }
    QMouseEvent * m = new QMouseEvent(type, QPoint(mouseEvent.X,mouseEvent.Y), initate, buttons, keys);
    QCoreApplication::postEvent(m_gui->ui ()->canvasWidget, m);
    return;
}

void FvController::PostKeyEvent (const PureWeb::Ui::PureWebKeyboardEventArgs &args)
{
    return;


    if(0){
        dbg(1) << "KeyEvent:\n"
               << "  KeyCode = " << args.KeyCode << "\n"
               << "  Char. code = " << args.CharacterCode << "\n"
               << "  type = " << args.EventType << "\n"
               << "  mods = " << args.Modifiers << "\n";
    }

    if( args.KeyCode == 32 && args.EventType == CSI::PureWeb::Ui::KeyboardEventType::KeyDown) {
        toggleCursorFrozen();
        //        fvView_->ui ()->debugWidget->append ( QString("Freezing %1\n").arg(isCursorFrozen ()));
    }

}

//void
//FvController::foobar (const CSI::ValueChangedEventArgs &args)
//{
////    QString path = QString( args.Path ().ToAscii ().begin ());
////    QString value = QString( args.NewValue ().ToAscii ().begin ());
////    fvView_->ui ()->debugWidget->append(
////                QString("Value changed: %1 %2 %3").arg(path).arg(value).arg(args.ChangeType ()));
//}


