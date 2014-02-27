/**
 * This is the license for the CyberSKA image viewer.
 *
 **/

#include "QUProfileController.h"

#include <cmath>
#include <QColor>
#include <QPainter>
#include <QFontMetricsF>
#include <QFont>

#include "FitsViewerLib/common.h"
#include "FitsViewerLib/FitsViewerServer.h"
#include "FitsViewerLib/ColormapFunction.h"

// helper function to help us create bindings for global variables
template <class T>
static void binder(
        QUProfileController * th,
        std::unique_ptr<GlobalState::TypedVariable<T> > & ptr,
        const QString & name)
{
    ptr.reset( new GlobalState::TypedVariable<T>( th-> pwPrefix() + name));
    std::function<void()> cb = std::bind(&QUProfileController::globalVariableCB, th, name);
    ptr->addCallback( cb);
}
template <class T>
static void binder(
        QUProfileController * th,
        std::unique_ptr<GlobalState::TypedVariable<T> > & ptr,
        const QString & name,
        const T & defaultValue)
{
    binder( th, ptr, name);
    ptr-> set( defaultValue);
}


/**
 * @brief constructor for QUProfileController
 * @param parent
 * @param wcsHelper - coordinate/value formatter
 * @param id
 * @param title
 * @param willDoFitting
 */
QUProfileController::QUProfileController(QObject * parent,
                                         const QString & id,
                                         const QString & title)
    : QObject(parent)
{
    m_myId = id;
    m_pwPrefix = QString( "/quprofile-%1/").arg( m_myId);
    m_viewName = QString( "quprofile-%1").arg( m_myId);
    m_initialized = false;

    // create a render buffer
    m_buffer = QImage( 2, 2, QImage::QImage::Format_RGB888);

    // setup fonts
    m_labelFont = QFont( "Helvetica", 8);
    m_captionFont = QFont( "Helvetica", 10, QFont::Normal);
    m_labelFontMetrics.reset( new QFontMetricsF( m_labelFont));
    m_captionFontMetrics.reset( new QFontMetricsF( m_captionFont));

    // register a view with pureweb
    CSI::PureWeb::Server::ViewImageFormat viewImageFormat;
    viewImageFormat.PixelFormat = CSI::PureWeb::PixelFormat::Rgb24;
    viewImageFormat.ScanLineOrder = CSI::PureWeb::ScanLineOrder::TopDown;
    viewImageFormat.Alignment = 4;
    GetStateManager().ViewManager().RegisterView(
                m_viewName.toStdString(), this);
    GetStateManager().ViewManager().SetViewImageFormat(
                m_viewName.toStdString(), viewImageFormat);
    GetStateManager ().ViewManager ().SetViewInteracting(
                m_viewName.toStdString(), false);

    // register PureWeb callback for all commands
    GetStateManager().CommandManager().AddUiHandler(
                ("quprofile-" + m_myId).toStdString(),
                CSI::Bind(this, & QUProfileController::commandDispatcher ));

    // cursor requests are arriving as state changes
    GetStateManager().XmlStateManager().AddValueChangedHandler(
                QString("/requests/quprofile-%1/mouseHover").arg(m_myId).toStdString(),
                CSI::Bind(this, &QUProfileController::mouseHoverRequestCB));

    // set some defaults
    m_total = 0;
    m_cursorIndex = -1;
    m_autoZoom = true;

    // setup the colormap function (cached)
    {
        ColormapFunction cmap = ColormapFunction::sunbow();
        cmap.setRgbScales( 0.5, 0.5, 0.5);
        HistogramColormapFunctor hfun( 0, 1, cmap);
        m_cmap.reset( new CachedRgbFunction( hfun, 0, 1, 256, Rgb(0,0,0)));
    }

    m_updateTimer = new QTimer( this);
    m_updateTimer->setSingleShot( true);
    m_updateTimer->setInterval(1);

    // set up the graph labelers
    m_vertLabeler = std::make_shared< Plot2dLabelers::BasicLabeler >();
//    m_vertLabeler-> setDual( false);
    m_horizLabeler = std::make_shared< Plot2dLabelers::BasicLabeler >();
//    m_horizLabeler-> setDual( false);

    // connect our update timer to callback
    connect( m_updateTimer, SIGNAL(timeout()), this, SLOT(updateTimerCB()));

    // create global variables reflecting the state of the object
    m_ignoreVarCallbacks = true;
    binder( this, m_vars.dotSize, "dotSize", 2.5);
    binder( this, m_vars.transparency, "transparency", 0.5);
    binder( this, m_vars.showCursor, "showCursor", true);
    binder( this, m_vars.showMean, "showMean", true);
    binder( this, m_vars.showGrid, "showGrid", true);
    binder( this, m_vars.showSunbow, "showSunbow", true);
    binder( this, m_vars.showConnectingLines, "showConnect", false);
    binder( this, m_vars.visibleOnClient, "visibleOnClient", false);

    // outgoing variables (no need to assign callbacks to these)
    m_vars.qVal.reset( new GlobalState::StringVar( m_pwPrefix + "q"));
    m_vars.uVal.reset( new GlobalState::StringVar( m_pwPrefix + "u"));
    m_vars.qMean.reset( new GlobalState::StringVar( m_pwPrefix + "qmean"));
    m_vars.uMean.reset( new GlobalState::StringVar( m_pwPrefix + "umean"));
    m_vars.frame.reset( new GlobalState::StringVar( m_pwPrefix + "frame"));
    m_vars.title.reset( new GlobalState::StringVar( m_pwPrefix + "title"));
    m_vars.title-> set( title);

    // unsuspend callback
    m_ignoreVarCallbacks = false;

    markDirty();
}

QUProfileController::~QUProfileController()
{

}

void QUProfileController::setData(
        quint64 total,
        const std::vector<double> & dataQ,
        const std::vector<double> & dataU)
{
    //    std::vector<double> dataQ = { 1,2,3 };
    //    std::vector<double> dataU= { 3,2,1 };
    //    total = 3;

    // make us a copy of the data
    m_total = total;
    m_dataQ = dataQ;
    m_dataU = dataU;

    // figure out data bounds & calculate means
    m_qmin = m_qmax = m_umin = m_umax = NAN;
    m_meanQ = m_meanU = 0;
    int count = 0;
    for( auto i = 0u ; i < avail() ; i ++) {
        double q = m_dataQ[i];
        if( ! std::isfinite(q)) continue;
        double u = m_dataU[i];
        if( ! std::isfinite(u)) continue;
        if( m_qmin > q || isnan( m_qmin)) m_qmin = q;
        if( m_umin > u || isnan( m_umin)) m_umin = u;
        if( m_qmax < q || isnan( m_qmax)) m_qmax = q;
        if( m_umax < u || isnan( m_umax)) m_umax = u;
        m_meanQ += q;
        m_meanU += u;
        count ++;
    }
    m_meanQ /= count;
    m_meanU /= count;

    // repaint everything
    markDirty();
}

const QString &QUProfileController::getId() const
{
    return m_myId;
}

void QUProfileController::setTitle(const QString &title)
{
    m_vars.title-> set( title);
//    m_title = title;
//    markDirty( Dirty::TITLE);
}

const QString &QUProfileController::getTitle() const
{
    return m_vars.title-> get();
//    return m_title;
}

void QUProfileController::setAutoZoom(bool val)
{
    m_autoZoom = val;
    markDirty();
}

// data to screen
double QUProfileController::ttx1(double x)
{
    //    return m_tx1.apply( x);
    return m_tx1( x);
}
double QUProfileController::tty1(double y)
{
    return m_ty1.apply(y);
}

// screen to data
double QUProfileController::ttx1inv(double x)
{
    //    return (x / m_buffer.width()) * (m_zoomX2 - m_zoomX1) + m_zoomX1;
    return m_tx1.inv(x);
}
double QUProfileController::tty1inv(double y)
{
    //    return (y / m_buffer.height()) * (m_zoomY2 - m_zoomY1) + m_zoomY1;
    return m_ty1.inv(y);
}

qint64 QUProfileController::avail() const
{
    return std::min( m_dataQ.size(), m_dataU.size());
}

void QUProfileController::commandDispatcher(Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
{
    //    dbg(1) << "g1dfcontroller: dispatcher\n";
    QString cmd = command["/cmd"].ValueOr<QString>("");

    if( cmd == "zoomRect") {
        double x1 = command["/val1"].ValueOr<double>( 0.0);
        double y1 = command["/val2"].ValueOr<double>( 0.0);
        double x2 = command["/val3"].ValueOr<double>( 1.0);
        double y2 = command["/val4"].ValueOr<double>( 1.0);
        // make sure we don't respond to empty zoom requests
        if( fabs(x1-x2) < 1e-6 || fabs(y1-y2) < 1e-6) return;
        m_zoomX1 = ttx1inv( x1);
        m_zoomY1 = tty1inv( y1);
        m_zoomX2 = ttx1inv( x2);
        m_zoomY2 = tty1inv( y2);
        swap_ordered( m_zoomX1, m_zoomX2);
        swap_ordered( m_zoomY2, m_zoomY1);
        m_autoZoom = false;
        markDirty();
    }
    else if( cmd == "rightClick") {
        if( getCursor() > -1)
            emit rightClick( getCursor());
    }
    else if( cmd == "zoomReset") {
        m_autoZoom = true;
        markDirty();
    }
    else {
        dbg(1) << ConsoleColors::warning() << "Unknown command: " << cmd
               << ConsoleColors::resetln();
    }
}

void QUProfileController::mouseHoverRequestCB(const ValueChangedEventArgs &args)
{
    std::string stdstr = args.NewValue().ConvertOr<std::string>("");
    QString s = stdstr.c_str();
    QStringList list = s.split( ' ', QString::SkipEmptyParts);
    // special case: null
    // this indicates the mouse has left the canvas on the client side so we'll set
    // the index to -1
    if( list.size() < 2 || list[0] == "null") {
        setCursor( -1);
        return;
    }
    // convert the parameters to integers
    double x, y;
    bool ok;
    x = list[0].toDouble( & ok);
    if( ok) y = list[1].toDouble( & ok);
    if( ! ok) {
        // something went wrong
        dbg(0) << "Warning: mouseHoverRequestCB received bad input: " << s;
        setCursor(-1);
        return;
    }
    // find the closest data point
    qint64 ind = -1;
    double minD = NAN;
    x = ttx1inv(x);
    y = tty1inv(y);
    auto av = avail();
    for( auto i = 0u ; i < av ; i ++ ) {
        double q = m_dataQ[i];
        if( ! std::isfinite(q)) continue;
        double u = m_dataU[i];
        if( ! std::isfinite(u)) continue;
        double d = (x-q) * (x-q) + (y-u) * (y-u);
        if( d < minD || ! std::isfinite(minD)) {
            ind = i;
            minD = d;
        }
    }
    setCursor( ind);
}

void QUProfileController::drawPlot(QPainter & p)
{
    p.save();
    p.setOpacity( m_vars.transparency-> get());
    p.setClipRect( m_x1, m_y1, m_x2 - m_x1, m_y2 - m_y1);
    p.setClipping(true);

    p.setPen( Qt::NoPen);
    QColor dotColor( 255, 0, 0); // if fixed colormap
    double x = 0.0, y = 0.0;
    //        for( size_t i = 0 ; i < m_data.size() ; i ++ ) {
    auto av = avail();
    for( auto i = 0u ; i < av ; i ++ ) {
        x = ttx1(m_dataQ[i]);
        y = tty1(m_dataU[i]);
        if( ! (std::isfinite(x) && std::isfinite(y))) { continue; }
        if( m_vars.showSunbow-> get() && m_total > 1) {
            dotColor = m_cmap-> qrgb( double(i) / (m_total - 1));
        }
        p.setBrush( dotColor);
        p.drawEllipse( QPointF(x,y), m_vars.dotSize-> get(), m_vars.dotSize-> get());
    }
    if( m_vars.showConnectingLines-> get()) {
        p.setOpacity(1.0);
        QPointF p1( 0, 0);
        bool validp1 = false;
        p.setPen( QPen( QColor(0,0,0), 0.25));
        //        p.setPen( QPen( QColor(0,0,0), 1));
        for( auto i = 0u ; i < av ; i ++ ) {
            QPointF p2( ttx1(m_dataQ[i]), tty1(m_dataU[i]));
            bool validp2 = std::isfinite(p2.x()) && std::isfinite(p2.y());
            if( validp2 && validp1) {
                p.drawLine( p1, p2);
            }
            p1 = p2;
            validp1 = validp2;
        }
    }
    p.restore();
}

void QUProfileController::drawGrid(QPainter & p)
{
    if( ! m_vars.showGrid-> get()) return;
    p.save();

    // text is outside of the plot area, so disable clipping
    p.setClipping(false);

    int labelFontHeight = m_labelFontMetrics-> height();
    //    int captionFontHeight = m_captionFontMetrics-> height();

    QColor captionColor( 0, 0, 128);

    // y axis labels
    int textGap = 2;
    p.setFont ( m_labelFont);
    p.setPen( QColor(100,100,100));
    for( auto label : m_vertLabels) {
        double yy = m_y2 - label.centerPix;
        p.drawText( QRectF( m_x2 + textGap, yy, 1, 1),
                    Qt::AlignVCenter | Qt::TextDontClip | Qt::AlignLeft,
                    label.txt1
                    );
    }

    // x axis labels
    for( Plot2dLabelers::LabelEntry & label : m_horizLabels) {
        double xx = m_x1 + label.centerPix;
        p.drawText( QRectF( xx, m_y2 + labelFontHeight / 2.0, 1, 1),
                    Qt::AlignCenter | Qt::TextDontClip,
                    label.txt1
                    );
    }

    // draw the grid lines
    // ===================
    p.setRenderHint( p.Antialiasing, false);
    //    p.setClipping( false);
    p.setClipRect( m_x1, m_y1, m_x2 - m_x1, m_y2 - m_y1);
    p.setClipping(true);
    p.setPen( QPen(QColor(255,0,0,20), 1.0));

    // draw horizontal lines
    for( auto label : m_vertLabels) {
        double yy = m_y2 - label.centerPix;
        p.drawLine( QPointF(m_x1, yy), QPointF(m_x2, yy));
    }

    // draw vertical lines
    for( auto label : m_horizLabels) {
        double xx = m_x1 + label.centerPix;
        p.drawLine( QPointF(xx, m_y1), QPointF(xx, m_y2));
    }

    // highlight 0 axes
    p.setPen( QPen(QColor(255,0,0, 128), 1.0));
    double x0 = ttx1( 0);
    double y0 = tty1( 0);
    p.drawLine( QPointF( x0, m_y1), QPointF( x0, m_y2));
    p.drawLine( QPointF( m_x1, y0), QPointF( m_x2, y0));

    // draw the surrounding rectangle
    p.setClipping( false);
    p.setPen( QPen( QColor( 255,70,70,128), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    p.drawRect( m_x1, m_y1, m_x2-m_x1, m_y2-m_y1);

    // put the painter in the original state
    p.restore();
}

void QUProfileController::drawLoadingMessage(QPainter & p)
{
    if( avail() < m_total || m_total == 0)
    {
        p.save();
        p.setFont ( m_captionFont);
        p.setOpacity(1);
        QString txt = QString( "Loading %1/%2").arg( avail()).arg(m_total);
        QRectF anchor( m_buffer.width()/2.0, m_buffer.height()/2.0, 1, 1);
        QRectF brect = p.boundingRect(
                    anchor,  Qt::AlignCenter | Qt::TextDontClip,
                    txt);
        brect.adjust( -3, -3, 3, 3);
        QColor bg( 255, 255, 255, 225);
        QColor border( 128, 0, 0, 225);
        p.setPen( border);
        p.setBrush( bg);
        p.drawRoundedRect( brect, 5, 5);
        p.setPen( QColor(0, 0, 0));
        p.drawText(anchor,  Qt::AlignCenter | Qt::TextDontClip,
                   txt);
        p.setPen( QPen(QColor( 0, 0, 0, 128), 1, Qt::DashLine));
        p.restore();
    }
}

/// recalculate and redraw everything that needs it, based on dirty flags
void QUProfileController::redrawAllNow()
{
    dbg(1) << "Redrawing quprofile #" << m_myId
           << " " << m_buffer.width() << "x" << m_buffer.height();

    double dotSize = m_vars.dotSize-> get();

//    bool updateDc = false;

    // helper function to format a double to a string
    auto fmtDouble = [](double x)->QString {
        if( std::isnan(x)) return "nan";
        if( std::isinf(x)) return "inf";
        return QString::number( x, 'g', 6);
    };

    int labelFontHeight = m_labelFontMetrics-> height();

    // apply autozoom
    if( m_autoZoom) {
        m_zoomX1 = m_qmin;
        m_zoomX2 = m_qmax;
        m_zoomY1 = m_umax; // max is at the top
        m_zoomY2 = m_umin;

        // make sure we are displaying something (cannot have x1==x2)
        if( m_zoomX2 - m_zoomX1 < 1e-9) {
            m_zoomX1 -= 1;
            m_zoomX2 += 1;
        }
        if( m_zoomY1 - m_zoomY2 < 1e-9) {
            m_zoomY2 -= 1;
            m_zoomY1 += 1;
        }
    }

    // if we are showing grid, make a first (bad) estimate for margins
    // so that we can adjust vertical zoom
    if( m_vars.showGrid-> get() && m_autoZoom) {
        m_x1 = 5; m_x2 = m_buffer.width() - measureLabelX( "0.0000001");
        m_y1 = 5; m_y2 = m_buffer.height() - labelFontHeight;
        double dw = std::fabs(m_zoomX2 - m_zoomX1);
        double dh = std::fabs(m_zoomY1 - m_zoomY2);
        double ww = std::fabs(m_x1 - m_x2);
        double wh = std::fabs(m_y1 - m_y2);
        // dw/dh = ww/wh
        if( std::fabs( dw/dh - ww/wh) > 1e-6) {
            if( dw/dh > ww/wh) {
                // a1 is wider, so make it taller
                double s = dw * wh / ww - dh;
                m_zoomY1 += s/2;
                m_zoomY2 -= s/2;
            } else {
                double s = ww * dh / wh - dw;
                m_zoomX1 -= s/2;
                m_zoomX2 += s/2;
            }
        }
    }



    // setup margins (m_x* and m_y*) & labels
    // note: this must happen simultaneously because the sizes of the
    // vertical axis labels will influence the drawing area...
    // ================================================================

    // start with vertical margins, they only depend on the font height, which does not change
    if( m_vars.showGrid-> get()) {
        m_y1 = 5;
        m_y2 = m_buffer.height() - labelFontHeight; // make room for bottom labels

        // update tty - it won't change anymore
        m_ty1 = LinearMap1D( m_zoomY1, m_zoomY2, m_y1+dotSize, m_y2-dotSize);

        // now calculate vertical labels
        // dont forget y1 = top, y2 = bottom
        m_vertLabeler-> setZoom( tty1inv( m_y2), tty1inv( m_y1));
        m_vertLabeler-> setPixels( m_y2 - m_y1 + 1);
        m_vertLabels = m_vertLabeler-> compute(
                    std::bind(& QUProfileController::measureLabelY, this,
                              std::placeholders::_1));

        // calculate the maximum width of all vertical labels
        double maxw = measureLabelX( "0");
        for( auto label : m_vertLabels) {
            maxw = std::max( maxw, measureLabelX( label.txt1));
        }
        maxw += measureLabelX( "0");

        // now we can set the horizontal margins
        m_x1 = 5;
        m_x2 = m_buffer.width() - maxw;
    }
    else {
        m_x1 = 1; m_x2 = m_buffer.width()-2;
        m_y1 = 1; m_y2 = m_buffer.height()-2;
        // update tty
        m_ty1 = LinearMap1D( m_zoomY1, m_zoomY2, m_y1+dotSize, m_y2-dotSize);
    }

    // fix aspect ratio by adjusting zoom x1,x2
    double dw = std::fabs(m_zoomX2 - m_zoomX1);
    double dh = std::fabs(m_zoomY1 - m_zoomY2);
    double ww = std::fabs(m_x1 - m_x2);
    double wh = std::fabs(m_y1 - m_y2);
    // dw/dh = ww/wh
    if( std::fabs( dw/dh - ww/wh) > 1e-6) {
        if( m_vars.showGrid-> get()) {
            // note: do not change y-zoom as we already used it to calculate vertical labels
            double s = ww * dh / wh - dw;
            m_zoomX1 -= s/2;
            m_zoomX2 += s/2;
        }
        else {
            // if we are not showing grid, we can decide if we adjust x or y zoom
            if( dw/dh > ww/wh) {
                // a1 is wider, so make it taller
                double s = dw * wh / ww - dh;
                m_zoomY1 += s/2;
                m_zoomY2 -= s/2;
                // re-update tty
                m_ty1 = LinearMap1D( m_zoomY1, m_zoomY2, m_y1+dotSize, m_y2-dotSize);
            } else {
                double s = ww * dh / wh - dw;
                m_zoomX1 -= s/2;
                m_zoomX2 += s/2;
            }
        }
    }

    // at this point margins are set, so update ttx*() functions
    m_tx1 = LinearMap1D( m_zoomX1, m_zoomX2, m_x1+dotSize, m_x2-dotSize);

    // calculate labels for the horizontal axis
    if( m_vars.showGrid-> get()) {
        m_horizLabeler-> setZoom( ttx1inv( m_x1), ttx1inv(m_x2));
        m_horizLabeler-> setPixels( m_x2 - m_x1 + 1);
        m_horizLabels = m_horizLabeler-> compute(
                    std::bind(& QUProfileController::measureLabelX, this,
                              std::placeholders::_1));
    }

    // do the drawing
    if( m_dirtyFlags.graph) {
        m_buffer.fill( 0xffffff);
        QPainter p( & m_buffer);
        p.setRenderHint( p.Antialiasing, true);
        p.setRenderHint( p.TextAntialiasing, true);
        p.setRenderHint( p.SmoothPixmapTransform, true);

        // set up clipping to only draw within the main plot area
        p.setClipRect( QRectF( QPointF(m_x1, m_y1), QPointF(m_x2, m_y2)));
        p.setClipping( true);

        // draw the grid
        drawGrid(p);

        // draw the plot
        drawPlot(p);

        // draw mean
        if( m_vars.showMean-> get()) {
            if( std::isfinite(m_meanQ) && std::isfinite(m_meanU)) {
                double x = ttx1( m_meanQ);
                double y = tty1( m_meanU);
                double s = 5;
                p.setPen( QPen( QColor(255,255,255,200), 5));
                p.drawLine( QPointF( x - s, y), QPointF( x + s, y));
                p.drawLine( QPointF( x , y - s), QPointF( x, y + s));
                s -= 1;
                if( m_vars.showSunbow-> get())
                    p.setPen( QPen( QColor(255,0,0), 3));
                else
                    p.setPen( QPen( QColor(0,0,128), 3));
                p.drawLine( QPointF( x - s, y), QPointF( x + s, y));
                p.drawLine( QPointF( x , y - s), QPointF( x, y + s));
            }
        }

        // draw cursor
//        if( m_showCursor && m_cursorIndex >= 0 && m_cursorIndex < avail()) {
        if( getShowCursor() && m_cursorIndex >= 0 && m_cursorIndex < avail()) {
            double x = m_dataQ[ m_cursorIndex];
            double y = m_dataU[ m_cursorIndex];
            x = ttx1(x);
            y = tty1(y);
            if( std::isfinite(x) && std::isfinite(y)) {
                p.setPen( QPen(QColor( 0,255,0), 2));
                p.setBrush( Qt::NoBrush);
                p.drawEllipse( QPointF(x,y), 6, 6);
            }
        }

        // if data is not complete yet, show a text
        p.setClipping( false);

        // let the clients know the mean
        m_vars.qMean-> set( fmtDouble( m_meanQ));
        m_vars.uMean-> set( fmtDouble( m_meanU));
//        updateDc = true;
    }

    // draw loading message if appropriate
    QPainter p( & m_buffer);
    drawLoadingMessage(p);

    // tell clients about the cursor
    if( m_dirtyFlags.cursor1) {
        if( m_cursorIndex >= 0 && m_cursorIndex < avail()) {
            m_vars.qVal-> set( fmtDouble( m_dataQ[ m_cursorIndex]));
            m_vars.uVal-> set( fmtDouble( m_dataU[ m_cursorIndex]));
        }
        else {
            m_vars.qVal-> set( "");
            m_vars.uVal-> set( "");
        }
        if( m_cursorIndex >= 0) {
            m_vars.frame-> set( QString::number( m_cursorIndex + 1));
        }
        else {
            m_vars.frame-> set( "");
        }
    }

//    // send additional info to render by the client
//    if( updateDc ){
//        pwsetdc( m_pwPrefix + "dc", qrand());
//    }

    //    dbg(1) << "... RedrawAllNow() done in " << debugTimer.elapsed() / 1000.0 << "s\n";
}

void QUProfileController::markDirty(Dirty dirty)
{
    switch (dirty) {
    case EVERYTHING:
        m_dirtyFlags.setAll();
        break;
    case CURSOR_INDEX:
        m_dirtyFlags.cursor1 = true;
        m_dirtyFlags.graph = true; // temporary, while we render this on server
        break;
//    case TITLE:
//        m_dirtyFlags.title = true;
//        break;
    default:
        break;
    }

    if( ! m_vars.visibleOnClient-> get()) {
        if(m_updateTimer->isActive())
            m_updateTimer->stop();
        return;
    }

    //    if(! m_updateTimer->isActive())
    //        m_updateTimer->start(0);

    int delay = 0;
    if( avail() < m_total) delay = 100;
    m_updateTimer->start(delay);

}

void QUProfileController::updateTimerCB()
{
    if( ! m_vars.visibleOnClient-> get()) return;

    // update m_buffer with the painting
    redrawAllNow();

    if( m_dirtyFlags.graph) {
        if( ! m_viewName.isEmpty()) {
            // TODO: deferred vs immediate - investigate which is better...
            GetStateManager().ViewManager().RenderViewDeferred( m_viewName.toStdString());
            //            GetStateManager().ViewManager().RenderViewImmediate( m_viewName.toStdString());
        }
    }

    // reset all dirty flags
    m_dirtyFlags = DirtyFlags();
}

void QUProfileController::SetClientSize(Size clientSize)
{
    //    dbg(1) << "Setting client size " << clientSize.Width << "x" << clientSize.Height;
    m_buffer = QImage( clientSize.Width, clientSize.Height, QImage::Format_RGB888);
    markDirty();
}


Size QUProfileController::GetActualSize()
{
    return CSI::PureWeb::Size(m_buffer.width(), m_buffer.height());
}

void QUProfileController::RenderView(RenderTarget image)
{
    if( ! m_vars.visibleOnClient-> get()) return;

    ByteArray bits = image.RenderTargetImage().ImageBytes();
    ByteArray::Copy(m_buffer.scanLine(0), bits, 0, bits.Count());

    //    // attach extra info to the frame
    //    CSI::Collections::Map<String,String> map = image.Parameters();
    //    map["xxyyzz"] = QString("%1 %2").arg(this->m_myId).arg(this->m_zoomY1).toStdString();
}

void QUProfileController::PostMouseEvent(const Ui::PureWebMouseEventArgs & /*mouseEvent*/)
{
}

QString QUProfileController::pwPrefix() const
{
    return m_pwPrefix;
}

//QString QUProfileController::formatAmplitudeWCS(double val)
//{
//    QString res;
//    bool appendUNIT = true;
//    if( m_formatterType == "RS-nan" || m_formatterType == "PIf"
//            || m_formatterType == "Qzf" || m_formatterType == "PA")
//        appendUNIT = false;
//    if( appendUNIT)
//        res = m_wcsHelper.formatValue( val);
//    else
//        res = QString::number( val);
//    return res;
//}

//QString QUProfileController::formatPositionWCS(double pos)
//{
//    QString unformattedAnswer = QString::number(pos) + " pix";
//    if( m_formatterType == "Ix") {
//        QStringList lst = m_wcsHelper.formatCursor( pos, m_dataCursor[1], m_dataCursor[2], false);
//        if( lst.size() < 1) return unformattedAnswer;
//        return lst[0];
//    }
//    if( m_formatterType == "Iy") {
//        QStringList lst = m_wcsHelper.formatCursor( m_dataCursor[0], pos, m_dataCursor[2], false);
//        if( lst.size() < 2) return unformattedAnswer;
//        return lst[1];
//    }
//    // all other plot types we have implemented so far are using the Z axes for data...
//    QStringList lst = m_wcsHelper.formatCursor( 0, 0, pos, false);
//    if( lst.size() < 3) return unformattedAnswer;
//    return lst[2];
//}

//static double calculateAngle( double a1, double b1, double a2, double b2)
//{
//    double x1 = sin(a1) * cos(b1);
//    double y1 = cos(a1) * cos(b1);
//    double z1 = sin(b1);

//    double x2 = sin(a2) * cos(b2);
//    double y2 = cos(a2) * cos(b2);
//    double z2 = sin(b2);

//    double dot = x1 * x2 + y1 * y2 + z1 * z2;
//    if( dot > 1) dot = 1;

//    return acos( dot);
//}

//static QString formatAngle( double rad)
//{
//    double deg = 180 * rad / M_PI;
//    int arcmin = floor( deg * 60);
//    deg -= arcmin / 60.0;
//    double arcsec = deg * 60 * 60;
//    QString res = "";
//    if( arcmin > 0) res = QString::number(arcmin) + "'";
//    res += QString::number(arcsec) + "\"";
//    return res;
//}

/*
QString QUProfileController::formatFwhmWCS(double fwhm, double pos)
{
    QString unformattedAnswer = QString::number(fwhm) + " pix";
    std::vector< WcsHelper::LabelAndUnit > lus = m_wcsHelper.getLabels();

    if( m_formatterType == "Ix" && lus.size() > 1) {
        std::vector<double> c1 = m_wcsHelper.toWorld( pos - fwhm/2, m_dataCursor[1], 0);
        std::vector<double> c2 = m_wcsHelper.toWorld( pos + fwhm/2, m_dataCursor[1], 0);
        double d = calculateAngle( c1[0], c1[1], c2[0], c2[1]);
        return formatAngle( d);
    }
    if( m_formatterType == "Ix" && lus.empty()) return unformattedAnswer;
    if( m_formatterType == "Iy" && lus.size() > 1) {
        std::vector<double> c1 = m_wcsHelper.toWorld( m_dataCursor[0], pos - fwhm/2, 0);
        std::vector<double> c2 = m_wcsHelper.toWorld( m_dataCursor[0], pos + fwhm/2, 0);
        double d = calculateAngle( c1[0], c1[1], c2[0], c2[1]);
        return formatAngle( d);
    }
    if( m_formatterType == "Iy" && lus.empty()) return unformattedAnswer;

    std::vector<double> c1 = m_wcsHelper.toWorld( 0, 0, pos - fwhm/2);
    std::vector<double> c2 = m_wcsHelper.toWorld( 0, 0, pos + fwhm/2);

    QString unit = "";
    if( lus.size() > 2 && ! lus[2].unitHtml.isEmpty())
        unit = " " + lus[2].unitHtml;

    return QString::number( fabs(c2[2] - c1[2])) + unit;
}
*/

void QUProfileController::globalVariableCB(const QString &name)
{
    // guard internals until constructor is finished
    if( m_ignoreVarCallbacks) return;

    dbg(1) << "qu gvar " << name << " changed.";
    if( name == "dotSize") {
        markDirty();
    }
    else if( name == "transparency") {
        markDirty();
    }
    else if( name == "showCursor") {
        dbg(1) << "val = " << m_vars.showCursor-> get();
        markDirty();
    }
    else {
        markDirty();
    }
}


double QUProfileController::measureLabelX(const QString &str)
{
    return m_labelFontMetrics-> width( str);
}

double QUProfileController::measureLabelY(const QString & /*str*/)
{
    return m_labelFontMetrics-> height();
}


void QUProfileController::setCursor(qint64 pos)
{
    if( m_cursorIndex == pos) return;
    m_cursorIndex = pos;
    emit cursorChanged(pos);
    markDirty( Dirty::CURSOR_INDEX);
}

void QUProfileController::setShowCursor(bool val)
{
//    m_showCursor = val;
    m_vars.showCursor->set( val);
    markDirty(Dirty::CURSOR_INDEX);
}

bool QUProfileController::getShowCursor()
{
//    return m_showCursor;
    return m_vars.showCursor-> get();
}

qint64 QUProfileController::getCursor() const
{
    if( m_cursorIndex < 0) return -1;
    return m_cursorIndex;
}


