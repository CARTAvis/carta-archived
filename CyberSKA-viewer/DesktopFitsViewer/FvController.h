/***
 * In theory: this is supposed to be the main controller for the whole application.
 *
 * In practice: it is the main controller, but it's not well designed at all. For example,
 * it functions as an IRenderView implementation for the main view. This is basically the
 * first class where experimental/new functionality finds it's place. Once the functionality
 * tested and desired, it is refactored into separate classes... A better mechanism is
 * needed to avoid this nasty cycle, e.g. well designed plugin system?
 */

#ifndef FVCONTROLLER_H
#define FVCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QMainWindow>
#include <QHash>

#include "MainWindow.h"
#include "FitsViewerLib/FitsViewerServer.h"
#include "FitsViewerLib/ColormapFunction.h"

#include "PureWeb.h"
#include "TimeoutMonitor.h"
#include "ui_MainWindow.h"
#include "FitsViewerLib/common.h"
#include "DebuggingDialog.h"
#include "FitsViewerLib/RaiCacherService.h"
#include "FitsViewerLib/RegionStatsService.h"
#include "FitsViewerLib/Gaussian2dFitService.h"
#include "Gauss1DFitterController.h"
#include "GlobalState.h"

//#include "FileBrowserService.h"
//#include "FitsViewerLib/FitsParser.h"
//#include "QUProfileService.h"

class ProfileController;
class FileBrowserService;


/// This is the main viewer controller class. It's a bit messy at the moment and could
/// use some serious re-factorization.
class FvController
        : public QObject
        , public CSI::PureWeb::Server::IRenderedView
{
    Q_OBJECT
public:
    explicit FvController( QStringList arguments);
    ~FvController();
    void start();

    void setDefaultFileToOpen(const FitsFileLocation &loc);

    // convert mouse coordinates to FITS, double-> double, and double-> int, and back
    QPointF mouse2fitsf( const QPointF & p);
    QPoint  mouse2fitsi( const QPointF & p);
    QPointF fits2mousef( const QPointF &);
    // convert mouse coordinates to image coordinates, double-> double, and double-> int
    QPointF mouse2imagef( const QPointF & p);
    QPoint  mouse2imagei( const QPointF & p);
    QPointF image2mousef( const QPointF &);
    // convert image coordinates to fits coordinates
    // (basically just flipping Y and adding 1 to both)
    QPointF image2fitsf( const QPointF & p);
    QPoint  image2fitsi( const QPointF & p);
    QPointF fits2imagef( const QPointF &);
    // sets the current frame immediately
    void setFrame( int frame);
    // sets histogram values
    void setHistogram( double min, double max);
    void setColormap( const ColormapFunction & );
    // cursor freezing
    bool isCursorFrozen( void) { return m_isCursorFrozen; }
    void setCursorFrozen( bool);

    // setter/getter for drawWidth
    inline const double & drawWidth() const { return m_drawWidth; }
    void setDrawWidth ( double v);
    // setter/getter for drawOrigin
    inline const QPointF & drawOrigin() const { return m_drawOrigin; }
    void setDrawOrigin( qreal x, qreal y );
    void setDrawOrigin( const QPointF & p);


    // IRenderedView
    void SetClientSize(CSI::PureWeb::Size clientSize);
    CSI::PureWeb::Size GetActualSize();
    void RenderView(CSI::PureWeb::Server::RenderTarget image);
    void PostKeyEvent(const CSI::PureWeb::Ui::PureWebKeyboardEventArgs& args);
    void PostMouseEvent(const CSI::PureWeb::Ui::PureWebMouseEventArgs& mouseEvent);

    // getter / setter for title
    void setTitle(const QString &title);
    QString title() const;

    /// general callback for global variables (it's public so that binder
    /// can access it)
    void globalVariableCB( const QString & name);
    QString globalCommandCB(const GlobalState::Command::CallbackParameter &p);

public slots:
    /// determine a file type and load the file
    void loadFileAuto( const FitsFileLocation & floc);

    /// listener for file browser load
    void fileBrowserLoadCB( QString);
    /// requests the current frame to be changed (multiple successive calls
    /// will result in only the last frame rendered)
    void requestFrame( int frame);
    /// toggle cursor freezing
    void toggleCursorFrozen( void);
    /// center functions
    void centerScreenPoint( QPointF p);
    void centerImagePoint( QPointF p);
    void zoomScreenRectangle( QPointF p1, QPointF p2);
    void zoomImageRectangle( QPointF p1, QPointF p2);

signals:
    void mainViewIsSynchronized();

protected slots:
    /// load the main file
    void loadMainFile( const FitsFileLocation & floc);
    /// load a single image
    void loadSingleImage( const FitsFileLocation & floc);
    /// load a cube (currently I, Q and U)
    void loadIQUset( const FitsFileLocation & floc);

    void canvasMouseMoveCB( QMouseEvent *);
    void canvasMousePressCB( QMouseEvent *);
    void canvasMouseReleaseCB( QMouseEvent *);
    void canvasMouseWheelCB( QWheelEvent *);
    void canvasKeyPressCB(QKeyEvent*);
    void canvasRepaint(); // only schedules a repaint
    void canvasRepaintRaw(); // does the actual repaint
    void colormapCB( ColormapFunction fn);
    // changes the current histogram values & repaints
    void histogramCB( double min, double max);
    void requestedFrameTimerCB(void);
    // zoom handler
    void zoomSliderCB( int);
    // sets pixel level zoom
    void setPixelZoom( double pzoom, QPoint pos);
    // conversions between drawWidth and pixelZoom and sliderZoom
    double drawWidth2pixelZoom( double dw);
    double pixelZoom2drawWidth( double pzoom);
    int drawWidth2sliderZoom( double dw);
    double sliderZoom2drawWidth( int szoom);
    // sets zoom based on value between 0-1000
    void setSliderZoom( int izoom, QPoint pos);
    /// performes necessary updates whenever anything changes regarding frozen cursor
    void updateFrozenCursor();
    /// update profiles dialogs
    void updateProfileDialogs();
    /// slot for receiving region stats updates from the region stats service
    void regionStatsServiceCB( RegionStatsService::ResultsRS);
    /// slot for receiving gaussian 2d fitting updates
    void gaussian2dFitServiceCB(Gaussian2dFitService::ResultsG2dFit);
    /// recompute visible image bounds
    void sendVisibleRectangle();
    /// slot for changed cached colormap
    void cachedColormapChangedCB();

protected:

    /// is a file loaded yet?
    bool m_isFileLoaded;

    /// the screen point that was clicked when iZoom was activated
    QPointF m_iZoomInitialScreenPt;
    /// the image point that was clicked when iZoom was activated
    QPointF m_iZoomInitialImagePt;
    /// the zoom level at the time iZoom was activated
    double m_iZoomInitialDrawWidth;

    // adjust origin to minimize blank space, meant to be used internally whenever
    // drawWidth or origin changes
    void minimizeBlankSpace();

    // Handlers
    void OnMinMaxChanged(const CSI::ValueChangedEventArgs& args);
    void loadFileShortcutCB(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void OnSetPixelZoom(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void OnSetSliderZoom(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void OnQuitApplication(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void OnCrosshair(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void OnCanvasClick(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void onViewUpdated(const CSI::ValueChangedEventArgs& args);
    void onCanvasMouseMove(const CSI::ValueChangedEventArgs& args);
    void OnCanvasRectangle(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void OnCanvasKeyDown(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void OnCanvasWheel(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void OnClientDebug(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void regionStatsRequestCB(const CSI::ValueChangedEventArgs& args);
    void g2dFitRequestCB(const CSI::ValueChangedEventArgs& args);
    void threeDplotRequestCB(const CSI::ValueChangedEventArgs& args);
    void qualityRequestCB(const CSI::ValueChangedEventArgs& args);
    void setWCSCommandCB(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);
    void serverLogCB(CSI::Guid sessionid, CSI::Typeless command, CSI::Typeless responses);

    // last known mouse position (real)
    QPointF lastKnownMousePosition_;
    void setLastKnownMousePosition( const QPointF &);
    const QPointF & lastKnownMousePosition() { return lastKnownMousePosition_; }

    // last known cursor position (affected by freezing)
    bool m_isCursorFrozen;
    QPoint m_lastCursorFitsPosition;

    // should be called when cursor position is changed
    // updates the text labels and profiles
    void  cursorChangedUpdate( void);


    // pointer to the GUI
    FvView * m_gui;

//    FitsParser fitsParser_;
    FitsViewerServer m_fvs;

    // where was the mouse pressed? used to determine panning
    QPointF canvasLastClickPosition_;
    // what was draw origin/width when the mouse was clicked?
    // it is used to determine panning & zoom
    QPointF dragDrawOrigin_;
    // copy of drawWidth when drag-zoom was started
    double dragDrawWidth_;

    // when frame requests are comming from the pureweb gui, but frame loading/rendering
    // takes 'long' time, these things get queued up. this timer based mechanism
    // only performs the last request
    int requestedFrame_;
    QTimer requestedFrameTimer_;

    // timeout monitor
    TimeoutMonitor timeoutMonitor_;

    // attributes affecting the rendering
    // TODO: this is basically a duplicate of the private data from FitsViewerServer,
    //       does it need to be duplicated???
    FitsFileLocation m_fLocation;
    int m_currentFrame;
    /// controls panning, i.e. the screen coordinate of the top left pixel of the image
    QPointF m_drawOrigin;
    /// scale control, contains number of horizontal pixels used to draw the entire image
    /// what a bad idea this was :)
    /// it should be something that is resolution invariant, e.g. pixel-zoom (how big
    /// is an image pixel on the screen?)
    double m_drawWidth;
    /// histogram clip values
    double m_histMin, m_histMax;
    ColormapFunction cmap_;
    // title of the window
    QString m_title;
    bool m_gridVisible;

    /// rendering buffer
    QImage m_buffer;
    /// pureweb view name
    QString m_pwViewName;

    /// location of a defult file to load (if main file fails?)
    /// This is a remnance of some debugging code, it should be removed.
    FitsFileLocation m_defaultImageLocation;

    // set if repaint is needed
    bool m_isRepaintNeeded;
    // last repaint ID (to detect that the last frame was rendered)
    qint64 m_lastRepaintId;

    // service for extracting z profiles
    ProfileController * m_profileController;

    // service for 2D gaussian fitting
    Gaussian2dFitService::Manager * m_g2dfitManager;

    // service for computing region stats
    RegionStatsService::Manager * m_rsManager;
    // last region stats parameters
    RegionStatsService::InputParametersRS rsInput_;
    RegionStatsService::ResultsRS rsResults_;
    Gaussian2dFitService::InputParametersG2dFit m_g2dInput;
    Gaussian2dFitService::ResultsG2dFit g2dResults_;

    // shared variables
    struct {
        std::auto_ptr<GlobalState::DoubleVar> iZoomRequest;
        std::auto_ptr<GlobalState::VDVar> iZoomInit;
    } m_vars;

    // file browser service
    QSharedPointer< FileBrowserService > m_fileBrowserService;

    // file shortcuts
    std::vector< std::string> m_fileShortcuts;

    // elgg hacks loaded in from the config file
    std::map< int, std::string > m_elggHacks;

    Q_DISABLE_COPY( FvController)

};

#endif // FVCONTROLLER_H
