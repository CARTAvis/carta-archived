/**
 * This is the license for the CyberSKA image viewer.
 *
 **/

/**
 * The purpose of this class is to draw QU plots.
 *
 * The controller listens for a PureWeb command:
 *   quprofile-<id>
 *
 * For state changes it listens for
 *   /requests/quprofile-<id>/...
 *
 */

#pragma once

#include "PureWeb.h"
#include "FitsViewerLib/LinearMap1D.h"
#include "FitsViewerLib/IGraphLabeler.h"
#include "GlobalState.h"
#include <QFont>
#include <QObject>
#include <QImage>
#include <QTimer>
#include <QFontMetricsF>
#include <memory>

using namespace CSI;
using namespace CSI::PureWeb;
using namespace CSI::PureWeb::Server;

class CachedRgbFunction;

class QUProfileController : public QObject, CSI::PureWeb::Server::IRenderedView
{
    Q_OBJECT

public:

    /**
     * @brief constructor
     * @param parent - Qt parent
     * @param wcsHelper - reference to a class to help format coordinates
     * @param prefix - prefix to use for setting pureweb state
     * @param id - id of the profile
     * @param pwViewname - pure web view name
     */
    explicit QUProfileController(QObject * parent,
            const QString & id,
            const QString & title
            );

    /**
     * @brief Destructor. Need non-default to use uniqe_ptr with forward declared types.
     */
    virtual ~QUProfileController();

    /**
     * @brief sets the data to be rendered
     * @param data
     */
    void setData( quint64 total = 0,
                  const std::vector<double> & dataQ = std::vector<double>(),
                  const std::vector<double> & dataU = std::vector<double>()
            );

    /**
     * @brief Return this controller's ID
     * @return
     */
    const QString & getId() const;

    /**
     * @brief setTitle
     * @param title
     */
    void setTitle( const QString & title);
    const QString & getTitle() const;

    void setAutoZoom( bool);
    quint64 getTotal() const { return m_total; }
    /**
     * @brief Sets the position of the cursor. Negative value hides it.
     */
    void setCursor( qint64 pos);
    /**
     * @brief show/hide cursor
     */
    void setShowCursor( bool val);
    /**
     * @brief Is the cursor visible?
     * @return yes/no
     */
    bool getShowCursor();
    /**
     * @brief Where is the cursor.
     * @return Index in the data (-1 implies no cursor)
     */
    qint64 getCursor() const;

    QString pwPrefix() const;

public slots:

signals:

    void cursorChanged( qint64);

protected:

    enum Dirty { EVERYTHING, CURSOR_INDEX };
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
    void mouseHoverRequestCB(const CSI::ValueChangedEventArgs& args);

    void initialGuessRequestCB(const CSI::ValueChangedEventArgs& args);

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

    // transformation helpers (data to/from screen coordinates)
    /// convert x_data to x_screen
    double ttx1( double x);
    /// convert y_data to y_screen
    double tty1( double y);
    /// convert x_screen to x_data
    double ttx1inv( double x);
    /// convert y_screen to y_data
    double tty1inv( double y);

    // shortcut to let us know the available data
    qint64 avail() const;

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
    bool m_initialized;
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
    std::vector<double> m_dataQ, m_dataU;

    /// total data to expect
    qint64 m_total;

    /// position of cursor 1 (index into m_data, -1 denotes no cursor)
    qint64 m_cursorIndex;
    /// value at cursor 1
    double m_cursor1value1, m_cursor1value2;

    /// current zoom parameters
    double m_zoomX1, m_zoomX2, m_zoomY1, m_zoomY2;

    /// calculated data bouds (min/max)
    double m_qmin, m_qmax, m_umin, m_umax;

    /// calculated mean
    double m_meanQ, m_meanU;

    /// margins for grid
    int m_y1, m_y2, m_x1, m_x2;

    /// which data is visible (starting & ending index)
    int m_i1, m_i2;
    /// data min/max
    double m_dataMinX, m_dataMinY, m_dataMaxX, m_dataMaxY;
    /// autozoom flags
    bool m_autoZoom;
    /// title of this fitter/profile
//    QString m_title;

    /// linear maps for convering data(x,y) to plot(x,y)
    LinearMap1D m_tx1, m_ty1;
    /// linear maps for convering data(x,y) to residual(x,y)
    LinearMap1D m_ty2;

    // cached colormap function
    std::unique_ptr< CachedRgbFunction > m_cmap;

    /// style parameters (via Global Variables API)
    struct {
        std::unique_ptr< GlobalState::DoubleVar >
        dotSize, transparency;
        std::unique_ptr< GlobalState::StringVar>
        qVal, qMean, uVal, uMean, frame, title;
        std::unique_ptr< GlobalState::BoolVar >
        showCursor, showMean, showGrid, showSunbow, showConnectingLines,
        visibleOnClient;
    } m_vars;

    // guard the callbacks from global variables until everything is constructed
    bool m_ignoreVarCallbacks/* = true*/;

    // update timer
    QTimer * m_updateTimer;

    // formatting helpers
//    QString formatAmplitudeWCS( double val);
//    QString formatPositionWCS( double pos);
//    QString formatFwhmWCS( double fwhm, double pos);

    /// fonts
    QFont m_labelFont, m_captionFont;
    std::unique_ptr< QFontMetricsF > m_labelFontMetrics, m_captionFontMetrics;

    /// pointers to vertical & horizontal labelers
    Plot2dLabelers::BasicLabeler::SharedPtr m_vertLabeler, m_horizLabeler;

    /// calculated labels
    std::vector< Plot2dLabelers::LabelEntry > m_vertLabels, m_horizLabels;

    /// general callback for all global variables (it's public so that binder
    /// can access it)
    void globalVariableCB( const QString & name);

    /// helper functions to measure labels using current font
    double measureLabelX( const QString & str);
    double measureLabelY( const QString & str);

signals:

    void rightClick( int);

protected slots:

    // update timer callback
    void updateTimerCB();
};
