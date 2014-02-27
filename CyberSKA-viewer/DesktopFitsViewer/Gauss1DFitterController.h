/**
 * The purpose of this class is to draw 2D plots for profiles, and to perform 1D gaussian
 * fitting.
 *
 * It is currently used to both render profiles, and to do gaussian 1D fitting. The fitting
 * part can be turned off by specifying willDoFitting=false in the constructor.
 *
 * TODO: refactor this into a ProfileRenderer and ProfileFitter, where ProfileFitter
 * would be a subclass of ProfileRenderer...
 *
 * The controller listens for a PureWeb command:
 *   g1df-<id>
 *
 * For state changes it listens for
 *   /requests/g1df-<id>/...
 *
 */

#ifndef GAUSS1DFITTERCONTROLLER_H
#define GAUSS1DFITTERCONTROLLER_H

#include <QFont>
#include <QObject>
#include <QImage>
#include <QTimer>
#include <QFontMetricsF>
#include <memory>
#include "PureWeb.h"
#include "FitsViewerLib/Gaussian1dFitService.h"
#include "FitsViewerLib/LinearMap1D.h"
#include "FitsViewerLib/IGraphLabeler.h"
#include "GlobalState.h"
#include "FitsViewerLib/WcsHelper.h"

using namespace CSI;
using namespace CSI::PureWeb;
using namespace CSI::PureWeb::Server;

class Gauss1DFitterController : public QObject, CSI::PureWeb::Server::IRenderedView
{
    Q_OBJECT

public:

    /**
     * @brief constructor
     * @param parent - Qt parent
     * @param wcsHelper - reference to a class to help format coordinates
     * @param id - id of the profile
     * @param title - dialog title
     * @param willDoFitting - whether this will be used for fitting or not
     */
    explicit Gauss1DFitterController(
            QObject * parent,
//            WcsHelper & wcsHelper,
            const QString & id,
            const QString & title,
            bool willDoFitting
            );

    /// set the vertical labeler
    void setVerticalLabeler( Plot2dLabelers::BasicLabeler::SharedPtr ptr);

    /// return the current vertical labeler
    Plot2dLabelers::BasicLabeler::SharedPtr verticalLabeler();

    /// set the horizontal labeler
    void setHorizontalLabeler( Plot2dLabelers::BasicLabeler::SharedPtr ptr);

    /// return the current horizontal labeler
    Plot2dLabelers::BasicLabeler::SharedPtr horizontalLabeler();


    /**
     * @brief Destructor. Need non-default to use uniqe_ptr with forward declared types.
     */
    virtual ~Gauss1DFitterController();

    /**
     * @brief sets the data to be rendered/fitted and the total to expect
     * @param data
     */
    void setData( quint64 total = 0, const std::vector<double> & data = std::vector<double>());

    /**
     * @brief Return this controller's ID
     * @return
     */
    const QString & getId() const;

    /**
     * @brief sets the title of the window
     * @param title
     */
    void setTitle( const QString & title);

    /**
     * @brief returns the title of the window
     */
    const QString & getTitle() const;

    void setAutoX( bool);
    bool getAutoX() const;
    void setAutoY( bool);
    bool getAutoY() const;
    void setPlotStyle(int ps);
    int getPlotStyle() const;
    quint64 getTotal() const { return m_total; }
    const std::vector<double> & getData() const { return m_data; }
    /**
     * @brief Sets the position of the cursor
     */
    void setCursor1( qint64 pos);
    void setCursor1mouse( qint64 pos);
//    void setShowCursor1( bool val);
//    bool getShowCursor1();
    qint64 getCursor1() const;
    void setDrawRms( bool);
    bool getDrawRms() const;
    void setShowGrid( bool);
    bool getShowGrid() const;
    void setXZoom( quint64 min, quint64 max);
    quint64 getXZoomMin() const { return m_zoomX1; }
    quint64 getXZoomMax() const { return m_zoomX2; }
    void setYZoom( double min, double max);
    double getYZoomMin() const;
    double getYZoomMax() const;
    double getYZoomVisibleMin() const;
    double getYZoomVisibleMax() const;
    void setXZoomLimits( quint64 min, quint64 max);
    void setYZoomAutoRange( double min, double max);

    /// general callback for all global variables (it's public so that binder
    /// can access it)
    void globalVariableCB( const QString & name);

public slots:

    void updateWcs();

signals:

    void cursorChanged( qint64);
    void autoXChanged( bool);
    void autoYChanged( bool);
    void xZoomChanged( quint64 min, quint64 max);
    void yZoomChanged( double min, double max);
    void visibleRangeChanged( double min, double max);
    void plotStyleChanged( int);
    void rightClick( int datax);

protected:

    enum Dirty { EVERYTHING, CURSOR1 };
    struct DirtyFlags {
        bool cursor1;
        bool graph;
        DirtyFlags() {
            cursor1 = false;
            graph = false;
        }
        void setAll() {
            cursor1 = true;
            graph = true;
        }
    };
    DirtyFlags m_dirtyFlags;

    // single entry point for all pureweb commands
    // here we dispatch the commands to the appropriate handlers below
    void commandDispatcher(Guid sessionid, Typeless command, Typeless responses);

    /// callback for requests for mouse cursor changes from the client
    void mouseCursor1CB(const CSI::ValueChangedEventArgs& args);


//    void commandInit(Guid sessionid, Typeless command, Typeless responses);
//    void commandUnzoom();
//    void commandZoomX(Guid sessionid, Typeless command, Typeless responses);
//    void commandZoomY(Guid sessionid, Typeless command, Typeless responses);
//    void commandSetRange(Guid sessionid, Typeless command, Typeless responses);
//    void commandSetPlotStyle(Guid sessionid, Typeless command, Typeless responses);


//    void initialGuessRequestCB(const CSI::ValueChangedEventArgs& args);

    void drawPlot(QPainter &p);
    void drawResidual(QPainter &p);
    void drawGrid(QPainter &p);
    void drawFitFuction(QPainter &p);
    void drawManipulationBars(QPainter &p);
    void drawLoadingMessage(QPainter & p);


    /// mark what's dirty and schedules a recompute/redraw
    void markDirty( Dirty dirty = EVERYTHING);

    /// do a redraw right now
    void redrawAllNow();

    /// set the range, number of gaussians/poly terms
    void setFitParameters(int left, int right, int ng, int np);
    /// set manual mode on/off
    void setManualMode( bool);
    /// show/hide manipulation bars
//    void showManipulationBars( bool);
    /// enable/disable use of random heuristics
    void enableRandomHeuristics( bool);
    /// make an evenly distributed initial guess
    void makeEvenlyDistributedInitialGuess();
    /// updates m_tx, m_ty
    /// should be called any time m_buffer.size(), m_zoom* or m_margin* change
    /// before using ttx,tty,ttxinv and ttyinv
//    void _updateTx();
    // transformation helpers (data to/from screen coordinates)
    /// convert x_data to x_screen
    double ttx1( double x);
    /// convert y_data to y_screen
    double tty1( double y);
    /// convert x_screen to x_data
    double ttx1inv( double x);
    /// convert y_screen to y_data
    double tty1inv( double y);

    // IRenderView interface
    /// PureWeb callback when the client view is resized
    void SetClientSize(CSI::PureWeb::Size clientSize);
    /// PureWeb calls this to find out what size we will render
    CSI::PureWeb::Size GetActualSize();
    /// PureWeb callback to do the actual rendering into its buffer
    void RenderView(CSI::PureWeb::Server::RenderTarget image);
    /// PureWeb keyboard callback
    void PostKeyEvent(const CSI::PureWeb::Ui::PureWebKeyboardEventArgs& /*args*/) {}
    /// PureWeb mouse callback
    void PostMouseEvent(const CSI::PureWeb::Ui::PureWebMouseEventArgs& mouseEvent);

    /// rendering buffer
    QImage m_buffer;
    /// internal id
    QString m_myId;
    /// prefix to use for pureweb state changes
    QString m_pwPrefix;
    /// PureWeb view name (must be unique)
    QString m_viewName;
    /// which formatter to use for coordinates/values
    QString m_formatterType;
    /// the position of the cursor in the image (used with formatter for RA/DEC...)
    std::vector < double > m_dataCursor;

    /// the raw data to plot/fit
    std::vector<double> m_data;

    /// total data to expect
    quint64 m_total;

    /// position of cursor 1 (index into m_data, -1 denotes no cursor)
    qint64 m_cursor1;
    /// value at cursor 1
    double m_cursor1value;

    // TODO: deprecate
    /// reference to wcs helper
//    WcsHelper & m_wcsHelper;

    /// current zoom parameters
    int m_zoomX1, m_zoomX2;
    double m_zoomY1, m_zoomY2;
    /// which data is visible (starting & ending index)
    int m_i1, m_i2;
    /// visible vertical range
    double m_visibleYmin, m_visibleYmax;
    /// horizontal zoom limits
    int m_zoomXmin, m_zoomXmax;
    /// vertical zoom starting range for auto mode
    double m_autoYmin, m_autoYmax;
    /// whether this instance will be used to do fitting at all
    bool m_willDoFitting;
    /// title of this fitter/profile
    QString m_title;

    /// margins for grid
    int m_y0, m_y1, m_y2, m_y3, m_y4, m_y5, m_x1, m_x2;

    /// linear maps for convering data(x,y) to plot(x,y)
    LinearMap1D m_tx1, m_ty1;
    /// linear maps for convering data(x,y) to residual(x,y)
    LinearMap1D m_ty2;

    /// style parameters
    int m_plotStyle;
    bool m_visibleOnClient;

    /// service for 1D gaussian fitting
    Gaussian1dFitService::Manager * m_g1dfitManager;

    /// copy of the input passed to g1d fitting
    Gaussian1dFitService::InputParametersG1dFit m_g1dInput;

    /// copy of the results received from g1d fitting
    Gaussian1dFitService::ResultsG1dFit m_g1dResults;

    /// initial guesses used in manual mode
    std::vector<Optimization::Gauss1dNiceParams> m_initialGuess;

    /// update timer
    QTimer * m_updateTimer;

    // formatting helpers
    QString formatAmplitudeWCS( double val);
    QString formatPositionWCS( double pos);
    QString formatFwhmWCS( double fwhm, double pos);

    /// helper to format a set of gaussians into a table
//    QString gaussiansToTable(std::vector<Optimization::Gauss1dNiceParams> & gs , QString heading);
    QString gaussiansToTable2(
            std::vector<Optimization::Gauss1dNiceParams> & gs,
            std::vector<Optimization::Gauss1dNiceParams> & gs2,
            QString heading);

    /// fonts
    QFont m_labelFont, m_captionFont, m_peakFont;
    std::unique_ptr< QFontMetricsF > m_labelFontMetrics, m_captionFontMetrics, m_peakFontMetrics;

    /// pointers to vertical & horizontal labelers
    Plot2dLabelers::BasicLabeler::SharedPtr m_vertLabeler, m_horizLabeler;

    /// calculated labels
    std::vector< Plot2dLabelers::LabelEntry > m_vertLabels, m_horizLabels;

    /// helper functions to measure labels using current font
    double measureLabelX( const QString & str);
    double measureLabelY( const QString & str);

    // shared variables
    struct {
        std::unique_ptr< GlobalState::StringVar > initialGuess;
        std::unique_ptr< GlobalState::VIVar > plotArea;
        std::unique_ptr< GlobalState::BoolVar > manualInitialConditions, randomHeuristics,
        showManipBars, showResidual, showPeakLabels, useWCS, showRms,
        autoX, autoY;
    } m_vars;

    // guard the callbacks from global variables until everything is constructed
    bool m_ignoreVarCallbacks /*= true*/;


protected slots:
    // callback for the 1d fitting service
    void gaussian1dFitServiceCB( Gaussian1dFitService::ResultsG1dFit r);

    // update timer callback
    void updateTimerCB();

    
};

#endif // GAUSS1DFITTERCONTROLLER_H
