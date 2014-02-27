#include "Gauss1DFitterController.h"

#include <cmath>
#include <QColor>
#include <QPainter>
#include <QFontMetricsF>
#include <QFont>

#include "FitsViewerLib/common.h"
#include "FitsViewerLib/Optimization/Gauss1d.h"
#include "FitsViewerLib/FitsViewerServer.h"
#include "FitsViewerLib/WcsHelper.h"

// helper function to help us create bindings for global variables
template <class T>
static void binder(
        Gauss1DFitterController * th,
        std::unique_ptr<GlobalState::TypedVariable<T> > & ptr,
        const QString & prefix,
        const QString & name)
{
    ptr.reset( new GlobalState::TypedVariable<T>(prefix + name));
    std::function<void()> cb = std::bind(&Gauss1DFitterController::globalVariableCB, th, name);
    ptr->addCallback( cb);
}
template <class T>
static void binderd(
        Gauss1DFitterController * th,
        std::unique_ptr<GlobalState::TypedVariable<T> > & ptr,
        const QString & prefix,
        const QString & name,
        const T & defaultValue)
{
    binder( th, ptr, prefix, name);
    ptr-> set( defaultValue);
}


/**
 * @brief Helper class to format an ASCII table
 */
class TextTable
{
public:
    enum Justification { Left, Right };

    TextTable() {
        m_maxCol = 0;
        m_maxRow = 0;
    }

    void setJustification( int col, Justification just) {
        m_just[ col] = just;
    }

    void set( int row, int col, QString s) {
        m_map[row][col] = s;
        if( m_widths.find( col) == m_widths.end())
            m_widths[col] = 0;
        m_widths[col] = std::max( m_widths[col], s.length());
        m_maxCol = std::max( m_maxCol, col);
        m_maxRow = std::max( m_maxRow, row);
    }
    QString toString() {
        QString res;
        QTextStream out( & res);
        for( int row = 0 ; row <= m_maxRow ; row ++) {
            for( int col = 0 ; col <= m_maxCol ; col ++ ) {
                QString val = m_map[row][col];
                if( m_just[col] == Left)
                    val = val.leftJustified( m_widths[col]);
                else
                    val = val.rightJustified( m_widths[col]);
                if( col > 0) out << "|";
                out << val;
            }
            out << "\n";
        }
        return res;
    }

protected:
    /// TODO: 2d array with map instead of vector, because originally I thought
    /// this would be used for bigger sparse tables. For small tables this
    /// should be changed to vectors...
    std::map< int, std::map< int, QString > > m_map;
    std::map< int, Justification > m_just;
    std::map< int, int> m_widths;
    int m_maxCol, m_maxRow;
};

/// fonts used during render
//struct Gauss1DFitterController::MyFonts {
//};


/**
 * @brief constructor for Gauss1DFitterController
 * @param parent
 * @param wcsHelper - coordinate/value formatter
 * @param id (must start with non number)
 * @param title
 * @param willDoFitting
 */
Gauss1DFitterController::Gauss1DFitterController(
        QObject * parent,
//        WcsHelper & wcsHelper,
        const QString & id,
        const QString & title,
        bool willDoFitting
        )
    : QObject(parent)
//    , m_wcsHelper( wcsHelper)
{
    m_myId = id;
    m_pwPrefix = QString( "/g1dfs/%1/").arg(m_myId);
    m_viewName = QString( "g1dview-%1").arg( m_myId);
    m_willDoFitting = willDoFitting;

    m_vertLabeler = std::make_shared< Plot2dLabelers::BasicLabeler >();
    m_horizLabeler = std::make_shared< Plot2dLabelers::BasicLabeler >();
//    m_horizLabeler-> setValtxf( [] (double v) -> double { return v + 1; });
//    m_horizLabeler-> setValtxf( LinearMap1D( 0, 1, 1, 2));
    m_horizLabeler-> setOffset( 1.0);
    m_horizLabeler-> setMinDelta( 1);

    // create a render buffer
    m_buffer = QImage( 2, 2, QImage::QImage::Format_ARGB32_Premultiplied);

    // setup fonts
    m_labelFont = QFont( "Helvetica", 8);
    m_captionFont = QFont( "Helvetica", 10, QFont::Normal);
    m_peakFont = QFont( "Courier", 8, QFont::Bold);
    m_labelFontMetrics.reset( new QFontMetricsF( m_labelFont));
    m_captionFontMetrics.reset( new QFontMetricsF( m_captionFont));
    m_peakFontMetrics.reset( new QFontMetricsF( m_peakFont));

    // register a view with pureweb
    CSI::PureWeb::Server::ViewImageFormat viewImageFormat;
    viewImageFormat.PixelFormat = CSI::PureWeb::PixelFormat::Bgrx32;
    viewImageFormat.ScanLineOrder = CSI::PureWeb::ScanLineOrder::TopDown;
    viewImageFormat.Alignment = 4;
    GetStateManager().ViewManager().RegisterView(
                m_viewName.toStdString(), this);
    GetStateManager().ViewManager().SetViewImageFormat(
                m_viewName.toStdString(), viewImageFormat);
    GetStateManager ().ViewManager ().SetViewInteracting(
                m_viewName.toStdString(), false);

    //    // register wheel callback too
    //    GetStateManager().CommandManager().AddUiHandler(
    //                (m_viewName + "-wheel").toStdString(),
    //                CSI::Bind(this, & Gauss1DFitterController::commandWheel ));

    //    // add callback for initial guess requests
    //    GetStateManager().XmlStateManager().AddValueChangedHandler(
    //                QString("/requests/g1dfit-%1/initGuess").arg(m_myId).toStdString(),
    //                CSI::Bind(this, &Gauss1DFitterController::initialGuessRequestCB));

    // register PureWeb callback for all commands
    GetStateManager().CommandManager().AddUiHandler(
                ("g1df-" + m_myId).toStdString(),
                CSI::Bind(this, & Gauss1DFitterController::commandDispatcher ));

    // cursor requests are arriving as state changes
    GetStateManager().XmlStateManager().AddValueChangedHandler(
                QString("/requests/g1df/%1/mouseCursor1").arg(m_myId).toStdString(),
                CSI::Bind(this, &Gauss1DFitterController::mouseCursor1CB));

    // create a 1d gauss fitting service
    if( m_willDoFitting) {
        m_g1dfitManager = new Gaussian1dFitService::Manager(this);
        // connect the gaussian 1d fitter service to our callbacks
        connect( m_g1dfitManager, SIGNAL(progress(Gaussian1dFitService::ResultsG1dFit)),
                 this, SLOT(gaussian1dFitServiceCB(Gaussian1dFitService::ResultsG1dFit)));
        connect( m_g1dfitManager, SIGNAL(done(Gaussian1dFitService::ResultsG1dFit)),
                 this, SLOT(gaussian1dFitServiceCB(Gaussian1dFitService::ResultsG1dFit)));
    }
    else {
        m_g1dfitManager = 0;
    }

    m_plotStyle = 1;
//    m_showResidual = m_willDoFitting;
//    m_showPeakLabels = m_willDoFitting;
//    m_showManipulationBars = m_willDoFitting;
//    m_showCursor1 = true;
//    m_useWCS = true;
    m_visibleOnClient = false;

    m_total = 0;
    m_cursor1 = -1;
//    m_autoX = true;
//    m_autoY = true;
    m_zoomXmin = -1; // indicating no zoom limits
    m_zoomXmax = -1;
    m_autoYmin = m_autoYmax = NAN;
//    m_showRms = false;

    m_updateTimer = new QTimer( this);
    m_updateTimer->setSingleShot( true);
    m_updateTimer->setInterval(1);

    // connect our update timer to callback
    connect( m_updateTimer, SIGNAL(timeout()), this, SLOT(updateTimerCB()));

    // create and bind global variables
    // ================================
    m_ignoreVarCallbacks = true;
    binderd( this, m_vars.initialGuess, m_pwPrefix, "initialGuess", QString(""));
    binderd( this, m_vars.manualInitialConditions, m_pwPrefix, "manualInitialConditions", false);
    binderd( this, m_vars.randomHeuristics, m_pwPrefix, "randomHeuristics", true);

    binderd( this, m_vars.showManipBars, m_pwPrefix, "showManipBars", m_willDoFitting);
    binderd( this, m_vars.showResidual, m_pwPrefix, "showResidual", m_willDoFitting);
    binderd( this, m_vars.showPeakLabels, m_pwPrefix, "showPeakLabels", false);
    binderd( this, m_vars.useWCS, m_pwPrefix, "useWCS", true);
    binderd( this, m_vars.showRms, m_pwPrefix, "showRms", false);
    binderd( this, m_vars.autoX, m_pwPrefix, "autoX", true);
    binderd( this, m_vars.autoY, m_pwPrefix, "autoY", true);

    // some variables we just want to set once (for defaults)
    // these are basically variables only of interest to clients, but they should
    // have only one origin for setting for the first time
    GlobalState::BoolVar( m_pwPrefix + "showCursor1").set( true);
    GlobalState::BoolVar( m_pwPrefix + "isFitter").set( m_willDoFitting);

    m_vars.plotArea.reset( new GlobalState::VIVar( m_pwPrefix + "plotArea"));
    // unsuspend callback
    m_ignoreVarCallbacks = false;

    m_vars.manualInitialConditions-> set( false);

    setTitle( title);

    markDirty();
}

void Gauss1DFitterController::setVerticalLabeler(Plot2dLabelers::BasicLabeler::SharedPtr ptr)
{
    m_vertLabeler = ptr;
    markDirty();
}

Plot2dLabelers::BasicLabeler::SharedPtr Gauss1DFitterController::verticalLabeler()
{
    return m_vertLabeler;
}

void Gauss1DFitterController::setHorizontalLabeler(Plot2dLabelers::BasicLabeler::SharedPtr ptr)
{
    m_horizLabeler = ptr;
    markDirty();
}

Plot2dLabelers::BasicLabeler::SharedPtr Gauss1DFitterController::horizontalLabeler()
{
    return m_horizLabeler;
}

Gauss1DFitterController::~Gauss1DFitterController()
{
    if( m_updateTimer) {
        m_updateTimer->deleteLater();
    }
}

void Gauss1DFitterController::setData(quint64 total, const std::vector<double> &data)
{
    // make us a copy of the data
    m_total = total;
    m_data = data;

    // reset parameters (1 gaussian, 2 polynomial terms)
    setFitParameters( 0, data.size(), 1, 2);

    // redraw
    markDirty();
}

const QString &Gauss1DFitterController::getId() const
{
    return m_myId;
}

void Gauss1DFitterController::setTitle(const QString &title)
{
    m_title = title;
    pwset( m_pwPrefix + "title", m_title);
}

const QString &Gauss1DFitterController::getTitle() const
{
    return m_title;
}

void Gauss1DFitterController::setAutoX(bool val)
{
    m_vars.autoX-> set( val);
    markDirty();
}

bool Gauss1DFitterController::getAutoX() const
{
    return m_vars.autoX-> get();
}

void Gauss1DFitterController::setAutoY(bool val)
{
    m_vars.autoY-> set( val);
    markDirty();
}

bool Gauss1DFitterController::getAutoY() const
{
    return m_vars.autoY-> get();
}

void Gauss1DFitterController::setPlotStyle(int ps)
{
    if( ps == m_plotStyle) return;
    m_plotStyle = ps;
    markDirty();
}

void Gauss1DFitterController::gaussian1dFitServiceCB( Gaussian1dFitService::ResultsG1dFit r)
{
    m_g1dResults = r;

    // sort the gaussians by center (left to right)

    struct Anon {
        double c, a, v;
        Anon() {}
        Anon( double * ptr) {
            c = ptr[0]; a = ptr[1]; v = ptr[2];
        }
        static bool cmp( const Anon & x, const Anon & y) { return x.c < y.c; }
    };

    if( r.status() != r.Error && r.input.nGaussians > 1) {
        std::vector< Anon > v;
        for( int i = 0 ; i < r.input.nGaussians ; i ++ ) {
            v.push_back( Anon( & r.params[i*3]));
        }

        std::sort( v.begin(), v.end(), Anon::cmp);

        for( int i = 0 ; i < r.input.nGaussians ; i ++ ) {
            r.params[i*3+0] = v[i].c;
            r.params[i*3+1] = v[i].a;
            r.params[i*3+2] = v[i].v;
        }
        m_g1dResults = r;
    }

    markDirty();
}

// data to screen
double Gauss1DFitterController::ttx1(double x)
{
    //    return m_tx1.apply( x);
    return m_tx1( x);
}
double Gauss1DFitterController::tty1(double y)
{
    return m_ty1.apply(y);
}

// screen to data
double Gauss1DFitterController::ttx1inv(double x)
{
    //    return (x / m_buffer.width()) * (m_zoomX2 - m_zoomX1) + m_zoomX1;
    return m_tx1.inv(x);
}
double Gauss1DFitterController::tty1inv(double y)
{
    //    return (y / m_buffer.height()) * (m_zoomY2 - m_zoomY1) + m_zoomY1;
    return m_ty1.inv(y);
}

//void Gauss1DFitterController::_updateTx()
//{
//    m_tx = LinearMap1D( m_zoomX1, m_zoomX2, m_leftMargin, m_buffer.width() - m_rightMargin);
//    m_ty = LinearMap1D( m_zoomY1, m_zoomY2, m_topMargin, m_buffer.height() - m_bottomMargin);

////    markDirty();
//}

//void Gauss1DFitterController::commandInit(Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
//{
//    //    dbg(1) << "g1dfcontroller: init\n";
//    if( ! m_initialized) {
//        m_initialized = true;
//        Nullable<QString> id = command["/id"].As<Nullable<QString> >();
//        m_myId = id.Value();

//        CSI::PureWeb::Server::ViewImageFormat viewImageFormat;
//        viewImageFormat.PixelFormat = CSI::PureWeb::PixelFormat::Rgb24;
//        viewImageFormat.ScanLineOrder = CSI::PureWeb::ScanLineOrder::TopDown;
//        viewImageFormat.Alignment = 4;
//        m_viewName = QString( "G1DFitView%1").arg( id.Value());
//        dbg(1) << "Registering G1Dcontroller for " << m_viewName << "\n";
//        GetStateManager().ViewManager().RegisterView(
//                    m_viewName.toStdString(), this);
//        GetStateManager().ViewManager().SetViewImageFormat(
//                    m_viewName.toStdString(), viewImageFormat);
//        GetStateManager ().ViewManager ().SetViewInteracting(
//                    m_viewName.toStdString(), false);

//    }

//    // set the data type
//    m_formatterType = command["/plotType"].ValueOr("").ToAscii().begin();
//    m_dataCursor.clear();
//    {
//        QString dataString = command["/cursor"].ValueOr<QString>("0_0_0");
//        QStringList dataList = dataString.split('_');
//        m_dataCursor.clear();
//        for( int i = 0 ; i < dataList.length() ; i ++ ) {
//            bool ok;
//            double val = dataList[i].toDouble( & ok);
//            if( ! ok) val = std::numeric_limits<float>::quiet_NaN();
//            m_dataCursor.push_back( val);
//        }
//        if( m_dataCursor.size() != 3) {
//            m_dataCursor.resize(3);
//            m_dataCursor[0] = 0;
//            m_dataCursor[1] = 0;
//            m_dataCursor[2] = 0;
//        }
//    }
//    dbg(1) << "Registering type " << m_formatterType
//           << " with cursor " << m_dataCursor[0] << ", " << m_dataCursor[1]
//           << ", " << m_dataCursor[2]
//           << "\n";

//    m_data.clear();
//    QString dataString = command["/data"].ValueOr<QString>("0");
//    QStringList dataList = dataString.split('_');
//    for( int i = 0 ; i < dataList.length() ; i ++ ) {
//        bool ok;
//        double val = dataList[i].toDouble( & ok);
//        if( ! ok) val = std::numeric_limits<float>::quiet_NaN();
//        m_data.push_back( val);
//    }

//    m_zoomXmin = command["/x1"].ValueOr<double>(0);
//    m_zoomXmax = command["/x2"].ValueOr<double>(1);
////    m_extraZoomY1 = command["/y1"].ValueOr<double>(0);
////    m_extraZoomY2 = command["/y2"].ValueOr<double>(1);
//    swap_ordered( m_zoomXmin, m_zoomXmax);
////    swap_ordered( m_extraZoomY2, m_extraZoomY1);

//    m_zoomX1 = m_zoomXmin;
//    m_zoomX2 = m_zoomXmax;
////    m_zoomY1 = m_extraZoomY1;
////    m_zoomY2 = m_extraZoomY2;

//    int ng = command["/ng"].ValueOr<int>(0);
//    int np = command["/poly"].ValueOr<int>(0);

//    setFitParameters( round(m_zoomX1), round(m_zoomX2), ng, np);
//    m_plotStyle = command["/plotStyle"].ValueOr<int>(0);

//    markDirty();
//}

//void Gauss1DFitterController::commandUnzoom()
//{
//    //    m_zoomX1 = m_initZoomX1;
//    //    m_zoomX2 = m_initZoomX2;
//    m_zoomX1 = 0;
//    m_zoomX2 = m_total;
//    m_zoomY1 = NAN;
//    m_zoomY2 = NAN;
//    for( size_t i = 0 ; i < m_data.size() ; i ++ ) {
//        if( isnan(m_zoomY1) || m_data[i] < m_zoomY1) m_zoomY1 = m_data[i];
//        if( isnan(m_zoomY2) || m_data[i] > m_zoomY2) m_zoomY2 = m_data[i];
//    }
//    if( isnan( m_zoomY1) || isnan( m_zoomY2)) {
//        m_zoomY1 = -1;
//        m_zoomY2 = 1;
//    }
//    std::swap( m_zoomY1, m_zoomY2);
//    markDirty();
//}

//void Gauss1DFitterController::commandZoomX(Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
//{
//    double x1 = command["/x1"].ValueOr<double>(0);
//    double x2 = command["/x2"].ValueOr<double>(1);

//    x1 = ttxinv(x1);
//    x2 = ttxinv(x2);

//    int x1i = std::floor(x1);
//    int x2i = std::ceil(x2);

//    swap_ordered( x1i, x2i);
//    setXZoom( x1i, x2i);
//    redrawLater();
//}

//void Gauss1DFitterController::commandZoomY(Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
//{
//    double y1 = command["/y1"].ValueOr<double>(0);
//    double y2 = command["/y2"].ValueOr<double>(1);

//    double ny1 = ttyinv(y1);
//    double ny2 = ttyinv(y2);

//    m_zoomY1 = ny1;
//    m_zoomY2 = ny2;

//    swap_ordered( m_zoomY2, m_zoomY1);
//    markDirty();
//}

void Gauss1DFitterController::commandDispatcher(Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
{
    //    dbg(1) << "g1dfcontroller: dispatcher\n";
    QString cmd = command["/cmd"].ValueOr<QString>("");

    if( cmd == "horizontalZoom") {
        qint64 pos1 = command["/val1"].ValueOr<qint64>( -1);
        qint64 pos2 = command["/val2"].ValueOr<qint64>( -1);
        swap_ordered( pos1, pos2);
        setXZoom( floor(ttx1inv( pos1)), std::ceil(ttx1inv( pos2)));
        emit xZoomChanged( quint64(m_zoomX1), quint64(m_zoomX2));
        markDirty();
    }
    else if( cmd == "verticalZoom") {
        qint64 pos1 = command["/val1"].ValueOr<qint64>( -1);
        qint64 pos2 = command["/val2"].ValueOr<qint64>( -1);
        swap_ordered( pos1, pos2);
        setYZoom( tty1inv( pos1), tty1inv( pos2));
        emit yZoomChanged( m_zoomY1, m_zoomY2);
        markDirty();
    }
//    else if( cmd == "autoX") {
//        bool val = command["/val1"].ValueOr<bool>( true);
//        setAutoX( val);
//        emit autoXChanged( m_autoX);
//        markDirty();
//    }
//    else if( cmd == "autoY") {
//        bool val = command["/val1"].ValueOr<bool>( true);
//        setAutoY( val);
//        emit autoYChanged( m_autoY);
//        markDirty();
//    }
//    else if( cmd == "showCursor1") {
//        bool val = command["/val1"].ValueOr<bool>( true);
//        setShowCursor1( val);
//    }
    else if( cmd == "plotType") {
        QString pt = command["/val1"].ValueOr<QString>( "join");
        if( pt == "join")
            setPlotStyle( 0);
        else if( pt == "hist")
            setPlotStyle( 1);
        else
            setPlotStyle( 2);
    }
    else if( cmd == "nGauss") {
        int ng = command["/val1"].ValueOr<int>( 0);
        setFitParameters( m_g1dInput.left, m_g1dInput.right, ng, m_g1dInput.poly);
    }
    else if( cmd == "nPoly") {
        int np = command["/val1"].ValueOr<int>( 0);
        setFitParameters( m_g1dInput.left, m_g1dInput.right, m_g1dInput.nGaussians, np);
    }
    else if( cmd == "rightClick") {
        qint64 x = command["/val1"].ValueOr<qint64>( -1);
        int ind = std::round(ttx1inv( x));
        ind = clamp( ind, 0, int(m_total)-1);
        emit rightClick( ind);
    }
//    else if( cmd == "heuristics") {
//        bool val = command["/val1"].ValueOr<bool>( false);
//        enableRandomHeuristics( val);
//    }
//    else if( cmd == "showManipulationBars") {
//        bool val = command["/val1"].ValueOr<bool>( false);
//        showManipulationBars( val);
//    }
//    else if( cmd == "manualInitialConditions") {
//        bool val = command["/val1"].ValueOr<bool>( false);
//        setManualMode( val);
//    }
    else if( cmd == "resetInitialConditions") {
        makeEvenlyDistributedInitialGuess();
    }
//    else if( cmd == "showResidual") {
//        m_showResidual = command["/val1"].ValueOr<bool>( false);
//        markDirty();
//    }
//    else if( cmd == "showLabelPeaks") {
//        m_showPeakLabels = command["/val1"].ValueOr<bool>( false);
//        markDirty();
//    }
//    else if( cmd == "useWCS") {
//        m_useWCS = command["/val1"].ValueOr<bool>( false);
//        markDirty();
//    }
    else if( cmd == "setVisible") {
        m_visibleOnClient = command["/val1"].ValueOr<bool>( false);
        markDirty();
    }
//    else if( cmd == "showRms") {
//        m_showRms = command["/val1"].ValueOr<bool>( false);
//        markDirty();
//    }
    else {
        dbg(1) << ConsoleColors::warning() << "Unknown command: " << cmd
               << ConsoleColors::resetln();
    }
}

void Gauss1DFitterController::mouseCursor1CB(const ValueChangedEventArgs &args)
{
    qint64 oldCursor = getCursor1();
    qint64 pos = args.NewValue().ConvertOr<qint64>(-1);
    setCursor1mouse( pos);
    // emit cursor change if the cursor changed
    if( oldCursor != getCursor1())
        emit cursorChanged( getCursor1());
}

/*
void Gauss1DFitterController::initialGuessRequestCB(const ValueChangedEventArgs &args)
{
    //    dbg(1) << "+initialGuessRequestCB\n";

    QString val = QString(args.NewValue().ValueOr("").ToAscii().begin());
    //    dbg(1) << "val = " << val << "\n";

    QStringList lst = val.split(' ', QString::SkipEmptyParts);
    //    dbg(1) << "n = " << lst.size() << "\n";

    m_initialGuess.clear();
    int ng = lst.size() / 3;
    bool allGood = true;
    for( int i = 0 ; i < ng ; i ++ ) {
        Optimization::Gauss1dNiceParams nice;
        bool ok;
        double cx = lst[i*3+0].toDouble( & ok);
        if( ! ok) { allGood = false; break; }
        double dx = lst[i*3+1].toDouble( & ok);
        if( ! ok) { allGood = false; break; }
        double ay = lst[i*3+2].toDouble( & ok);
        if( ! ok) { allGood = false; break; }
        if( ! std::isfinite(cx)) { allGood = false; break; }
        if( ! std::isfinite(dx)) { allGood = false; break; }
        if( ! std::isfinite(ay)) { allGood = false; break; }

        nice.center = ttx1inv( cx);
        nice.fwhm = 2 * (ttx1inv( cx + dx) - ttx1inv( cx));
        nice.amplitude = tty1inv( ay);

        m_initialGuess.push_back( nice);
    }
    if( ! allGood) m_initialGuess.clear();

    // restart the guessing with whatever we put into the initial guess
    setFitParameters( m_g1dInput.left, m_g1dInput.right, m_g1dInput.nGaussians, m_g1dInput.poly);
    markDirty();

    //    dbg(1) << "-initialGuessRequestCB\n";

}
*/

//void Gauss1DFitterController::commandSetRange(Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
//{
//    // extract the command parameters
//    double x1 = command["/x1"].ValueOr<double>(0);
//    double x2 = command["/x2"].ValueOr<double>(1);
//    int ng = command["/ng"].ValueOr<int>(1);
//    int poly = command["/poly"].ValueOr<int>(1);

//    //    dbg(1) << "pw setrange " << m_myId << ": " << x1 << " " << x2 << " " << ng << " " << poly << "\n";

//    // convert screen coordinates to data coordinates
//    double nx1 = round( ttxinv( x1));
//    double nx2 = round( ttxinv( x2));
//    setFitParameters( nx1, nx2, ng, poly);
//}

//void Gauss1DFitterController::commandSetPlotStyle(Guid /*sessionid*/, Typeless command, Typeless /*responses*/)
//{
//    m_plotStyle = command["/plotStyle"].ValueOr<int>(0);

//    markDirty();
//}




void Gauss1DFitterController::drawPlot(QPainter & p)
{
    p.save();

    p.setOpacity(1);
    if( m_plotStyle == 0) {
        QColor col1( 192, 192, 192, 255);
        QColor col2( 0, 0, 0, 255);
        QPen pen1( col1, 1);
        QPen pen2( col2, 1);
        QPointF pt;
        bool needMove = true;
        double x, y;
        //        for( size_t i = 0 ; i < m_data.size() ; i ++ ) {
        for( auto i = m_i1 ; i <= m_i2 ; i ++ ) {
            x = ttx1(i);
            y = tty1(m_data[i]);
            if( isnan(y)) { needMove = true; continue; }
            if( needMove) {
                pt = QPointF( x, y);
                needMove = false;
                continue;
            }
            QPointF pt2(x,y);
            //            if( int(i) > m_g1dResults.input.left && int(i) <= m_g1dResults.input.right)
            p.setPen( pen2);
            //            else
            //                p.setPen( pen1);
            p.drawLine( pt, pt2);
            pt = pt2;
        }
    }
    else if( m_plotStyle == 1) {
//        p.setRenderHint( p.Antialiasing, false);
        QColor col1( 192, 192, 192, 255);
        QColor col2( 0, 0, 0, 255);
        QPen pen1( col1, 1);
        QPen pen2( col2, 1);
        p.setPen( pen2);
        double lasty;
        bool lastyvalid = false;
        double x, y;
        double dx = ttx1(1) - ttx1(0);
        //        for( size_t i = 0 ; i < m_data.size() ; i ++ ) {
        for( auto i = m_i1 ; i <= m_i2 ; i ++ ) {
            x = ttx1(i);
            y = tty1(m_data[i]);
            if( isnan(y)) { lastyvalid = false; continue; }
            if( ! lastyvalid) {
                lastyvalid = true;
            } else {
                p.drawLine( QPointF( x - dx, lasty), QPointF( x - dx/2, lasty));
                p.drawLine( QPointF( x - dx/2, lasty), QPointF( x - dx/2, y));
                p.drawLine( QPointF( x - dx/2, y), QPointF( x, y));
            }
            lasty = y;
        }
    }
    else {
//        p.setRenderHint( p.Antialiasing, false);
        //        p.setPen( QPen( QColor("black"), 0.5));
        p.setPen( Qt::NoPen);
        QColor col1( 0, 0, 0, 64);
        QColor col2( 0, 0, 0, 200);
        p.setPen( col2);
        double x, y;
        double s = 4;
        for( auto i = m_i1 ; i <= m_i2 ; i ++ ) {
            x = ttx1(i);
            y = tty1(m_data[i]);
            if( isnan(y)) { continue; }
            p.drawLine( QPointF( x - s, y), QPointF( x + s, y));
            p.drawLine( QPointF( x, y - s), QPointF( x , y + s));
        }
    }
    p.restore();
}

void Gauss1DFitterController::drawResidual(QPainter & p)
{
    // if there is no data, do nothing
    if( m_data.size() == 0) return;

    // if residual display is disabled, do nothing
    if(! m_vars.showResidual-> get()) return;

    // if the result of the fitter is an error, do nothing
    if( m_g1dResults.status() == m_g1dResults.Error) return;

    // disable clipping
    p.setClipping( false);

    p.fillRect( m_x1, m_y4, m_x2 - m_x1, m_y5-m_y4, QColor( 0, 0, 255, 20 ));
    p.setPen( QPen( QColor( 0,0,128, 128), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    p.drawRect( m_x1, m_y4, m_x2 - m_x1, m_y5-m_y4);


    // calculate the array of residuals & record min/max
    double resMin = NAN, resMax = NAN;
    std::vector<double> resArray;
    resArray.resize( m_data.size());
    for( int i = m_i1 ; i <= m_i2 ; i ++ ) {
        double val = m_data[i] - Optimization::evalNGauss1dBkg(
                    i,
                    m_g1dResults.input.nGaussians,
                    m_g1dResults.input.poly,
                    m_g1dResults.params);

        resArray[i] = val;
        if( std::isnan(resMin) || val < resMin) resMin = val;
        if( std::isnan(resMax) || val > resMax) resMax = val;
    }

    // set up linear transformation for drawing the residual
    m_ty2 = LinearMap1D( resMin, resMax, m_y4+5, m_y5-5);

    // vertical 'Residual' label
    {
        p.save();
        p.setPen( QColor( 128, 128, 128));
        p.setFont( m_captionFont);
        QRect rec(0, 0, m_y5-m_y4, m_x1);
        p.translate( 0, m_y5);
        p.rotate( -90);
        //        p.fillRect( rec, QColor(0,0,255,128));
        p.drawText( rec,
                    Qt::AlignTop | Qt::TextDontClip | Qt::AlignHCenter,
                    "Residual");
        p.restore();
    }

    // draw min/max/0 labels
    {
        p.save();
        p.setClipRect( QRectF( QPointF(0, m_y4), QPointF(m_buffer.width(), m_y5)));
        p.setClipping( true);
        p.setPen( QColor( 128, 128, 128));
        p.setFont( m_labelFont);
        p.drawText( QRect(m_x2+2,m_y4,m_buffer.width()-m_x2-2,10),
                    Qt::AlignTop | Qt::TextDontClip | Qt::AlignLeft,
                    QString::number( resMax));
        p.drawText( QRect(m_x2+2,m_y5-10,m_buffer.width()-m_x2-2,10),
                    Qt::AlignBottom | Qt::TextDontClip | Qt::AlignLeft,
                    QString::number( resMin));
        p.drawText( QRect(m_x2+2,m_ty2(0)-10/2,m_buffer.width()-m_x2-2,10),
                    Qt::AlignVCenter | Qt::TextDontClip | Qt::AlignLeft,
                    "0");
        p.restore();
    }


    // set clipping to residual graph boundaries
    p.setClipRect( QRectF( QPointF(m_x1, m_y4), QPointF(m_x2, m_y5)));
    p.setClipping( true);

    // draw the y=0 horizontal line
    p.setPen( QColor( 100, 100, 100));
    p.drawLine( 0, m_ty2(0), m_buffer.width(), m_ty2(0));


    QColor col2( 0, 0, 128, 255);
    if( m_plotStyle == 0) {
        QPen pen2( col2, 1);
        p.setPen( pen2);
        QPointF pt;
        bool needMove = true;
        double x, y;
        //        for( int i = m_g1dResults.input.left ; i <= m_g1dResults.input.right ; i ++ ) {
        for( int i = m_i1 ; i <= m_i2 ; i ++ ) {
            x = ttx1(i);
            //            y = tty1(m_data[i] - Optimization::evalNGauss1dBkg(
            //                        i,
            //                        m_g1dResults.input.nGaussians,
            //                        m_g1dResults.input.poly,
            //                        m_g1dResults.params));
            //            y = m_ty2(m_data[i] - Optimization::evalNGauss1dBkg(
            //                        i,
            //                        m_g1dResults.input.nGaussians,
            //                        m_g1dResults.input.poly,
            //                        m_g1dResults.params));
            y = m_ty2(resArray[i]);
            if( isnan(y)) { needMove = true; continue; }
            y = clamp( y, - m_buffer.height() * 1000.0, m_buffer.height() * 1000.0);
            if( needMove) {
                pt = QPointF( x, y);
                needMove = false;
                continue;
            }
            QPointF pt2(x,y);
            p.drawLine( pt, pt2);
            pt = pt2;
        }
    }
    else {
        QPen pen2( col2, 1);
        p.setPen( pen2);
        double lasty;
        bool lastyvalid = false;
        double x, y;
        double dx = ttx1(1) - ttx1(0);
        for( int i = m_g1dResults.input.left ; i <= m_g1dResults.input.right ; i ++ ) {
            x = ttx1(i);
            //            y = tty1(m_data[i] - Optimization::evalNGauss1dBkg(
            //                        i,
            //                        m_g1dResults.input.nGaussians,
            //                        m_g1dResults.input.poly,
            //                        m_g1dResults.params));
            //            y = m_ty2(m_data[i] - Optimization::evalNGauss1dBkg(
            //                        i,
            //                        m_g1dResults.input.nGaussians,
            //                        m_g1dResults.input.poly,
            //                        m_g1dResults.params));
            y = m_ty2(resArray[i]);
            if( isnan(y)) { lastyvalid = false; continue; }
            y = clamp( y, - m_buffer.height() * 1000.0, m_buffer.height() * 1000.0);
            if( ! lastyvalid) {
                lastyvalid = true;
            } else {
                p.drawLine( QPointF( x - dx, lasty), QPointF( x - dx/2, lasty));
                p.drawLine( QPointF( x - dx/2, lasty), QPointF( x - dx/2, y));
                p.drawLine( QPointF( x - dx/2, y), QPointF( x, y));
            }
            lasty = y;
        }
    }

}

void Gauss1DFitterController::drawGrid(QPainter & p)
{
    p.save();

    // clip to the main graph
    p.setClipRect( m_x1, m_y1, m_x2 - m_x1, m_y2 - m_y1);
    p.setClipping(false);

    int labelFontHeight = m_labelFontMetrics-> height();
    //    int captionFontHeight = m_captionFontMetrics-> height();

    QColor captionColor( 128, 128, 128);

    // draw horizontal caption
    {
        p.setFont( m_captionFont);
        p.setPen( captionColor);
        p.drawText(
                    QRectF( m_x1, 0, m_x2 - m_x1, m_y0),
                    Qt::AlignCenter | Qt::TextDontClip,
                    m_horizLabeler-> caption2()
                    );
    }

    // draw vertical caption
    {
        p.save();
        p.setFont( m_captionFont);
        p.setPen( captionColor);
        QRect rec(0, 0, m_y2-m_y1, m_x1);
        p.translate( 0, m_y2);
        p.rotate( -90);
        p.drawText( rec,
                    Qt::AlignCenter | Qt::TextDontClip,
                    m_vertLabeler-> caption1());
        p.restore();
    }

    // draw labels for the vertical axis
    int textGap = 2;
    p.setFont ( m_labelFont);
    p.setPen( QColor(100,100,100));
    for( auto & label : m_vertLabels) {
        double yy = m_y2 - label.centerPix;
        p.drawText( QRectF( m_x2 + textGap, yy, 1, 1),
                    Qt::AlignVCenter | Qt::TextDontClip | Qt::AlignLeft,
                    label.txt1
                    );
    }

    // draw labels for the horizontal axis
    for( auto & label : m_horizLabels) {
        double xx = m_x1 + label.centerPix;
        p.drawText( QRectF( xx, m_y5 + labelFontHeight / 2.0, 1, 1),
                    Qt::AlignCenter | Qt::TextDontClip,
                    label.txt1
                    );
        p.drawText( QRectF( xx, m_y0 + labelFontHeight / 2.0, 1, 1),
                    Qt::AlignCenter | Qt::TextDontClip,
                    label.txt2
                    );
    }


    // draw grid lines
    // ===================
    p.setRenderHint( p.Antialiasing, false);
    p.setClipRect( m_x1, m_y1, m_x2 - m_x1, m_y2 - m_y1);
    p.setClipping(true);
    p.setPen( QPen(QColor(255,0,0,20), 1.0));

    // horizontal lines
    for( auto & label : m_vertLabels) {
        double yy = m_y2 - label.centerPix;
        p.drawLine( QPointF(m_x1, yy), QPointF(m_x2, yy));
    }

    // draw vertical lines
    for( auto & label : m_horizLabels) {
        double xx = m_x1 + label.centerPix;
        p.drawLine( QPointF(xx, m_y1), QPointF(xx, m_y2));
    }

    // highlight y=0
    p.setPen( QPen(QColor(255,0,0, 128), 1.0));
    p.drawLine( m_x1, tty1(0), m_x2, tty1(0));

    // draw the outline
    p.setClipping( false);
    p.setPen( QPen( QColor( 128,0,0,128), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    p.drawRect( m_x1, m_y1, m_x2-m_x1, m_y2-m_y1);

    // we need grid lines for the residual as well
    if( m_vars.showResidual-> get()) {
        p.setPen( QPen(QColor(255,0,0,20), 1.0));
        for( auto & label : m_horizLabels) {
            double xx = m_x1 + label.centerPix;
            p.drawLine( xx, m_y4, xx, m_y5);
        }
    }

    // restore the state
    p.restore();
}

void Gauss1DFitterController::drawFitFuction(QPainter& p)
{
    QVector<QPointF> pts;
    if( m_g1dResults.status() != m_g1dResults.Error) {
        int nSamp = ttx1(m_g1dResults.input.right) - ttx1(m_g1dResults.input.left);
        nSamp = clamp( 2 * nSamp, 10, (m_buffer.width()+1) * 10 );
        double dx = double(m_g1dResults.input.right - m_g1dResults.input.left)/nSamp;
        for( int i = 0 ; i <= nSamp ; i ++ )
        {
            double x = m_g1dResults.input.left + i * dx;
            double y = Optimization::evalNGauss1dBkg(
                        x,
                        m_g1dResults.input.nGaussians,
                        m_g1dResults.input.poly,
                        m_g1dResults.params);
            x = ttx1(x);
            y = tty1(y);
            if( (! std::isfinite(x)) || (! std::isfinite(y))) {
                // we cannot have nans/inf in drawPolyline
                pts.clear();
                break;
            }
            // clamp the y anyways, because apparently Qt does not like really huge
            // numbers. clamping should not really produce any visible artifacts
            y = clamp( y, - m_buffer.height() * 1000.0, m_buffer.height() * 1000.0);
            pts.append( QPointF( x, y));
        }

        if( false && ! pts.isEmpty()) {
            p.setPen( Qt::NoPen);
            p.setBrush( QColor(255, 0, 0, 64));
            QPolygonF polygon( pts);
            polygon << QPointF( ttx1(m_g1dResults.input.right), tty1(0));
            polygon << QPointF( ttx1(m_g1dResults.input.left), tty1(0));
            p.drawPolygon( polygon);
        }
    }

    if( m_g1dResults.status() != m_g1dResults.Error) {
        if( pts.isEmpty()) {
            QString txt = "Cannot render this fit";
            p.setFont ( m_captionFont);
            QRectF r( ttx1(m_g1dResults.input.left + m_g1dResults.input.right)/2,
                      m_buffer.rect().center().y(),
                      1, 1);
            QRectF br = p.boundingRect( r, Qt::AlignCenter | Qt::TextDontClip, txt);
            br.adjust( -5, -5, 5, 5);
            p.fillRect( br, QColor(0,0,0,255 * 0.1));
            p.setPen( QColor( 255, 64, 64));
            p.drawText(r, Qt::AlignCenter | Qt::TextDontClip, txt);
        }
        else
        {
            // draw the current fit
            p.save();
            QColor color("red");
            color.setAlphaF( 0.5);
            if( m_g1dResults.status() == m_g1dResults.Complete) {
                p.setPen( QPen( color, 4, Qt::SolidLine));
            } else {
                //                color.setAlphaF( 0.5);
                p.setPen( QPen( color, 4, Qt::DashLine));
            }
            p.drawPolyline( & pts[0], pts.size());
            p.restore();

            // for each peak, draw a center line and an optional label
            color.setAlphaF( 0.5);
            p.setPen( QPen( color, 1, Qt::DashLine));
            for( int i = 0 ; i < m_g1dResults.input.nGaussians ; i ++ ) {
                double xc = m_g1dResults.params[i*3+0];
                double y = Optimization::evalNGauss1dBkg( xc, m_g1dResults.input.nGaussians,
                                                          m_g1dResults.input.poly,
                                                          m_g1dResults.params);
                // draw the vertical dashed line to show the center of this peak
                if( std::isfinite(xc) && std::isfinite(y)) {
                    p.drawLine( QPointF(ttx1(xc), tty1(0)),
                                QPointF(ttx1(xc), tty1(y)));
                    // draw the label for this peak
                    if( m_vars.showPeakLabels-> get()) {
                        p.save();
                        p.setFont( m_peakFont);
                        p.setOpacity(1);
                        Optimization::Gauss1dNiceParams nice =
                                Optimization::Gauss1dNiceParams::convert(
                                    & m_g1dResults.params[i*3]);
                        QString txt;
                        QTextStream out( & txt);
                        out << "Gauss #" << i+1 << ":\n";
                        if( m_vars.useWCS-> get()) {
                            out << "Ampl: " << formatAmplitudeWCS( nice.amplitude) << "\n"
                                << "Posi: " << formatPositionWCS(nice.center) << "\n"
                                << "Fwhm: " << formatFwhmWCS(nice.fwhm,nice.center);
                        } else {
                            out << "Ampl: " << nice.amplitude << "\n"
                                << "Posi: " << nice.center << "\n"
                                << "Fwhm: " << nice.fwhm;
                        }
                        QRectF anchor( ttx1(xc), tty1(y) - 11, 1, 1);
                        QRectF brect = p.boundingRect(
                                    anchor,  Qt::AlignBottom | Qt::AlignLeft | Qt::TextDontClip,
                                    txt);
                        brect.adjust( -3, -3, 3, 3);
                        double dy = 0;
                        double dx = - brect.width() * 1.5;
                        if( brect.top() < m_y1) {
                            dy = - brect.top() + m_y1;
                        }
                        if( brect.left() + dx < 0) dx = brect.width() * 0.5;
                        brect.adjust(dx,dy,dx,dy);
                        anchor.adjust(dx,dy,dx,dy);
                        QColor bg( 255, 255, 255, 225);
                        QColor border( 128, 0, 0, 225);
                        p.setPen( border);
                        p.setBrush( bg);
                        p.drawRoundedRect( brect, 5, 5);
                        p.setPen( QColor(0, 0, 0));
                        p.drawText(anchor,  Qt::AlignBottom | Qt::AlignLeft | Qt::TextDontClip,
                                   txt);
                        p.setPen( QPen(QColor( 0, 0, 0, 128), 1, Qt::DashLine));
                        if( dx < 0) {
                            p.drawLine( QPointF( ttx1(xc), tty1(y)),
                                        QPointF( brect.right(), brect.center().y()));
                        } else {
                            p.drawLine( QPointF( ttx1(xc), tty1(y)),
                                        QPointF( brect.left(), brect.center().y()));
                        }
                        p.restore();
                    }
                }
            }
        }
    }
}

void Gauss1DFitterController::drawLoadingMessage(QPainter & p)
{
    if( m_data.size() < m_total || m_total == 0)
    {
        p.save();
        p.setFont ( m_captionFont);
        p.setOpacity(1);
        QString txt = QString( "Loading %1/%2").arg( m_data.size()).arg(m_total);
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

void Gauss1DFitterController::drawManipulationBars(QPainter & p)
{
    if( ! m_vars.showManipBars-> get()) return;

    p.save();
    for( size_t i = 0 ; i < m_initialGuess.size() ; i ++ )
    {
        Optimization::Gauss1dNiceParams & nice = m_initialGuess[i];

        double centerx, fwhmdx, amply;
        amply = tty1(nice.amplitude);
        centerx = ttx1(nice.center);
        fwhmdx = ttx1(nice.center + nice.fwhm/2) - ttx1(nice.center);

        if( false) {
            p.setPen( QPen( QBrush( QColor("green")), 2));
            p.drawLine( QPointF( centerx-fwhmdx, amply), QPointF( centerx+fwhmdx,amply));
            QString txt;
            QTextStream out( & txt);
            out << "#" << i+1;
            p.setFont ( m_peakFont);
            QRectF anchor( centerx, amply - 12, 1, 1);
            QRectF brect = p.boundingRect(
                        anchor,  Qt::AlignBottom | Qt::AlignHCenter | Qt::TextDontClip,
                        txt);
            brect.adjust( -3, -3, 3, 3);
            //                    brect.adjust(0,-5, 0,-5); anchor.adjust( 0, -5, 0, -5);
            QColor bg( 255, 255, 255, 225);
            QColor border( 0, 64, 0, 64);
            p.setPen( border);
            p.setBrush( bg);
            p.drawRect( brect);
            p.setPen( QColor(0, 64, 0));
            p.drawText(anchor,  Qt::AlignBottom | Qt::AlignHCenter | Qt::TextDontClip,
                       txt);
        }

        // draw the gaussian function for this initial guess
        std::vector<double> params = nice.convertToUgly();
        QVector<QPointF> pts;
        double left = nice.center - nice.fwhm/2.0;
        double right = nice.center + nice.fwhm/2.0;
        left = clamp( left, ttx1inv(0), ttx1inv(m_buffer.width()));
        right = clamp( right, ttx1inv(0), ttx1inv(m_buffer.width()));
        int nSamp = ttx1(right) - ttx1(left);
        //        dbg(1) << "ig nsamp = " << nSamp << "\n";
        nSamp = clamp( 2 * nSamp, 10, (m_buffer.width()+1) * 10 );
        double dx = (right - left)/nSamp;
        for( int i = 0 ; i <= nSamp ; i ++ )
        {
            double x = left + i * dx;
            double y = Optimization::evalGauss1d( x, & params[0]);
            x = ttx1(x);
            y = tty1(y);
            if( (! std::isfinite(x)) || (! std::isfinite(y))) {
                // we cannot have nans/inf in drawPolyline
                pts.clear();
                break;
            }
            // clamp the y anyways, because apparently Qt does not like really huge
            // numbers. clamping should not really produce any visible artifacts
            y = clamp( y, - m_buffer.height() * 1000.0, m_buffer.height() * 1000.0);
            pts.append( QPointF( x, y));
        }
        //        p.setPen( QPen( QColor(0,255,0,64), 4));
        p.setPen( QPen( QColor(0,128,0,64), 3));
        p.drawPolyline( & pts[0], pts.size());
    }

    p.restore();
}

/// recalculate and redraw everything that needs it, based on dirty flags
void Gauss1DFitterController::redrawAllNow()
{
    dbg(1) << "Redrawing profile " << m_myId;

    // figure out current horizontal zoom if in auto mode
    if( m_vars.autoX-> get()) {
        if( m_zoomXmin == -1) {
            // no limits were set on horizontal zoom, so select all current & future data
            m_zoomX1 = 0;
            m_zoomX2 = m_total-1;
        } else {
            // apply horizontal zoom limits
            m_zoomX1 = m_zoomXmin;
            m_zoomX2 = m_zoomXmax;
        }
    }

    // set the i1/i2 indices into m_data to match the horizontal zoom
    if( m_data.size() == 0) {
        m_i1 = 0; m_i2 = -1;
    }
    else {
        m_i1 = clamp<int>( m_zoomX1, 0, m_data.size()-1);
        m_i2 = clamp<int>( m_zoomX2, 0, m_data.size()-1);
    }

    // calculate the visible vertical range, and if it changed emit a signal
    {
        double oldVis[] { m_visibleYmin, m_visibleYmax};
        m_visibleYmin = NAN;
        m_visibleYmax = NAN;
        for( auto i = m_i1 ; i <= m_i2 ; i ++ ) {
            const auto & val = m_data[i];
            if( std::isnan( val)) continue;
            if( std::isnan(m_visibleYmin)) {
                m_visibleYmin = m_visibleYmax = val;
            }
            else {
                m_visibleYmin = std::min( m_visibleYmin, val);
                m_visibleYmax = std::max( m_visibleYmax, val);
            }
        }
        // if all data is essentially NANs, set the visible range to -1..+1
        if( ! std::isfinite(m_visibleYmin || ! std::isfinite(m_visibleYmax))) {
            m_visibleYmin = -1;
            m_visibleYmax = 1;
        }
        double newVis[] { m_visibleYmin, m_visibleYmax};
        if( 0 != std::memcmp( & oldVis, & newVis, sizeof( newVis))) {
            emit visibleRangeChanged( m_visibleYmin, m_visibleYmax);
        }
    }

    // update vertical zoom if in automatic mode
    if( m_vars.autoY-> get()) {
        if( std::isfinite( m_autoYmin))
            m_zoomY1 = std::min( m_visibleYmin, m_autoYmin);
        else
            m_zoomY1 = m_visibleYmin;
        if( std::isfinite( m_autoYmax))
            m_zoomY2 = std::max( m_visibleYmax, m_autoYmax);
        else
            m_zoomY2 = m_visibleYmax;

        if( ! std::isfinite( m_zoomY1)) {
            m_zoomY1 = -1.0;
            m_zoomY2 =  1.0;
        }
        // swap because y1 is the top, y2 is the bottom...
        std::swap( m_zoomY1, m_zoomY2);
    }

    // calculate mean and rms of the visible data
    double mean, rms;
    {
        double sum = 0.0;
        int n = 0;
        for ( auto i = m_i1 ; i <= m_i2 ; i++ )
        {
            if( ! std::isfinite( m_data[i])) continue;
            sum += m_data[i];
            n ++;
        }
        mean = sum / n;
        sum = 0.0;
        for ( auto i = m_i1 ; i <= m_i2 ; i++ )
        {
            if( ! std::isfinite( m_data[i])) continue;
            double d = m_data[i] - mean;
            sum += d * d;
        }
        rms = std::sqrt( sum / (n - 1));
        if( n < 2) rms = NAN;
    }

    // setup margins (m_x* and m_y*)
    // ================================================================

    // start with vertical margins
    //
    // first figure out space used for sure by labels & gui elements
    int captionFontHeight = m_captionFontMetrics-> height();
    int labelFontHeight = m_labelFontMetrics-> height();
    int initCondSpace = m_vars.manualInitialConditions-> get() ? 10 : 0;
    initCondSpace += m_vars.showResidual-> get() ? 5 : 0;
    int usedSpace = captionFontHeight + 2 * labelFontHeight + initCondSpace;
    int unusedSpace = m_buffer.height() - usedSpace;
    if( unusedSpace < 50) {
        // this is a problem, isn't it...
        // we'll just shift evrything way down't and hopefuly user will try
        // to resize the window to see everything
        unusedSpace = 50;
    }
    // residual height will be 0 if not shown, or 1/3 of the available space, but
    // at most 100 pixels
    int residualHeight = 0;
    if( m_vars.showResidual-> get()) {
        residualHeight = std::min<int>( unusedSpace / 3, 100);
    }
    // that leaves us with main graph height
    int mainGraphHeight = unusedSpace - residualHeight;

    m_y0 = captionFontHeight;
    m_y1 = m_y0 + labelFontHeight;
    m_y2 = m_y1 + mainGraphHeight;
    m_y3 = m_y2 + initCondSpace;
    m_y4 = m_y3 + 0; // y4 is not really used for anything...
    m_y5 = m_y4 + residualHeight;
    dbg(1) << "remaining space = " << m_buffer.height() - m_y5 - labelFontHeight;

    // update Y transform
    m_ty1 = LinearMap1D( m_zoomY1, m_zoomY2, m_y1 +5, m_y2-5);

    // calculate vertical labels
    m_vertLabeler-> setZoom( tty1inv( m_y2), tty1inv( m_y1));
    m_vertLabeler-> setPixels( m_y2 - m_y1 + 1);
    try {
        m_vertLabels.clear();
        m_vertLabels = m_vertLabeler-> compute(
                    std::bind(& Gauss1DFitterController::measureLabelY, this,
                              std::placeholders::_1));
    } catch ( std::exception & e) {
        warn() << "v_compute exception: " << e.what();
    } catch ( ... ) {
        warn() << "v-compute unknown exception";
    }

    // get the maximum width of all vertical labels
    double maxw = measureLabelX( "0");
    for( auto & label : m_vertLabels) {
        maxw = std::max( maxw, measureLabelX( label.txt1));
    }
    maxw += measureLabelX( "0");
    // make sure to account for residual
    if( m_vars.showResidual-> get()) {
        maxw = std::max( maxw, measureLabelX( "-0.00010"));
    }

    // use the max. width of the vertical labels to set the horizontal margins
    m_x1 = captionFontHeight;
    m_x2 = m_buffer.width() - maxw;

    // update ttx*() functions
    m_tx1 = LinearMap1D( m_zoomX1, m_zoomX2, m_x1, m_x2);

    // margins set, let the clients know
    m_vars.plotArea-> set({ m_x1, m_x2, m_y1, m_y2 });

    // calculate labels for the horizontal axis
    m_horizLabeler-> setZoom( ttx1inv( m_x1), ttx1inv(m_x2));
    m_horizLabeler-> setPixels( m_x2 - m_x1 + 1);
    try {
        m_horizLabels.clear();
        m_horizLabels = m_horizLabeler-> compute(
                    std::bind(& Gauss1DFitterController::measureLabelX, this,
                              std::placeholders::_1));
    } catch ( std::exception & e) {
        warn() << "h_compute exception: " << e.what();
    } catch ( ... ) {
        warn() << "h-compute unknown exception";
    }

    // find out the value at the cursor
    double cursor1mousePos = ttx1( m_cursor1);
    double cursor1valueMousePos = 0.0;
    if( m_cursor1 >= m_i1 && m_cursor1 <= m_i2 ) {
        m_cursor1value = m_data[ m_cursor1];
        cursor1valueMousePos = tty1( m_cursor1value);
    } else {
        m_cursor1value = INFINITY;
        cursor1valueMousePos = INFINITY;
    }

    // do the drawing
    if( m_dirtyFlags.graph) {
        //        dbg(1) << "Repainting " << m_myId << "\n";
        // draw
        m_buffer.fill( 0xffffff);
        QPainter p( & m_buffer);
        p.setRenderHint( p.Antialiasing, true);
        p.setRenderHint( p.TextAntialiasing, true);
        p.setRenderHint( p.SmoothPixmapTransform, true);

        // draw the grid
        drawGrid(p);

        // set up clipping to only draw within the main plot area
        p.setClipRect( QRectF( QPointF(m_x1, m_y1), QPointF(m_x2, m_y2)));
        p.setClipping( true);

        // draw the plot
        drawPlot(p);

        //    if( m_mouseX >= 0) {
        //        p.setPen( QColor("red"));
        //        p.drawLine( m_mouseX, 0, m_mouseX, m_buffer.height());
        //    }

        // draw the current fit function
        drawFitFuction(p);

        // draw the mean/rms
        if( m_vars.showRms-> get()) {
            double y1 = tty1( mean - rms);
            double y2 = tty1( mean + rms);
            double y12 = (y1+y2) / 2;
            if( std::isfinite(y1) && std::isfinite(y2)) {
                p.fillRect( QRectF( QPointF(m_x1,y1),QPointF(m_x2,y2)), QColor(0,255,0,40));
                p.setPen( QPen( QColor( 0, 128, 0, 128), 1, Qt::DashLine));
                p.drawLine( QPointF( m_x1, y12), QPointF( m_x2, y12));
            }
        }

        // draw manual initial guesses
        drawManipulationBars(p);

        // draw the cursor1 vertical line and the red dot for the value
        //        if( m_cursor1 >= 0 && getShowCursor1()) {
        //            if( std::isfinite(cursor1mousePos)) {
        //                int xi = std::round( cursor1mousePos);
        //                p.setPen( QPen( QColor( 0,0,255,64), 2));
        //                p.drawLine( xi, 0, xi, m_buffer.height());

        //                if( std::isfinite(m_cursor1value)) {
        //                    if( std::isfinite(cursor1valueMousePos)) {
        //                        p.setPen( Qt::NoPen);
        //                        p.setBrush( QColor( 255,0,0,128));
        //                        p.drawEllipse( QPointF(cursor1mousePos,cursor1valueMousePos), 3, 3);
        //                    }
        //                }
        //            }
        //        }

        // draw the residual
        drawResidual(p);

        // if data is not complete yet, show a text
        p.setClipping( false);
        drawLoadingMessage(p);
    }

    // send additional info to render by the client
    {
//        std::vector< WcsHelper::LabelAndUnit > lus = m_wcsHelper.getLabels();
//        if( lus.size() < 1) {
//            WcsHelper::LabelAndUnit lu;
//            lu.labelTxt = lu.labelHtml = "X";
//            lu.unitTxt = lu.unitHtml = "pix";
//            lus.push_back( lu);
//        }
//        if( lus.size() < 2) {
//            WcsHelper::LabelAndUnit lu;
//            lu.labelTxt = lu.labelHtml = "Y";
//            lu.unitTxt = lu.unitHtml = "pix";
//            lus.push_back( lu);
//        }
//        if( lus.size() < 3) lus.resize(3);

        QString s;
        if( m_g1dResults.input.isNull) {
            s = "Results: n/a (no input)";
        } else {
            QTextStream out(&s);

            int ng = m_g1dResults.input.nGaussians;
            int np = m_g1dResults.input.poly;

            std::vector<double> & params = m_g1dResults.params;

            std::vector<Optimization::Gauss1dNiceParams> gs;
            for( int i = 0 ; i < ng ; i ++ ) {
                Optimization::Gauss1dNiceParams nice =
                        Optimization::Gauss1dNiceParams::convert( & params[i*3]);
                gs.push_back( nice);
            }
            //            out << gaussiansToTable( gs, "Results:");
            out << gaussiansToTable2( gs, m_initialGuess, "Results:");

            if( np > 0) {
                out << "Polynomial terms:\n  ";
                for( int i = 0 ; i < np ; i ++ ) {
                    out << params[ng*3 + i];
                    if( i == 0) out << " ";
                    else if( i == 1) out << "x";
                    else out << " x^" << i;
                    if( i < np-1) out << " + ";
                }
                out << "\n";
            } else {
                out << "Polynomial terms: n/a\n";
            }
            out << "Residual RMS: ";
            if( m_vars.useWCS-> get())
                out << formatAmplitudeWCS( m_g1dResults.rms);
            else
                out << QString::number( m_g1dResults.rms);
            out << ", ";
            out << "Selected: "
                << (m_g1dResults.input.right - m_g1dResults.input.left + 1)
                << " data points";
        }
        pwset( m_pwPrefix + "fitResults", s.trimmed());

        // update client with the initial guess info (in screen coordinates!!!)
        {
            QString str;
            for( auto & nice : m_initialGuess) {
                double centerx, fwhmdx, amply;
                amply = tty1(nice.amplitude);
                centerx = ttx1(nice.center);
                fwhmdx = ttx1(nice.center + nice.fwhm/2) - ttx1(nice.center);
                if( ! str.isEmpty()) str.append( ' ');
                str += QString("%1 %2 %3").arg(centerx).arg(fwhmdx).arg(amply);
            }
            m_vars.initialGuess-> set( str);
        }

        if( m_cursor1 < 0) {
            pwset( m_pwPrefix + "cursor1/pos", "n/a");
            pwset( m_pwPrefix + "cursor1/val", "n/a");
            pwset( m_pwPrefix + "cursor1/mpos", "");
            pwset( m_pwPrefix + "cursor1/mval", "");
        }
        else {
            pwset( m_pwPrefix + "cursor1/pos", m_horizLabeler-> format( m_cursor1));
            QString valStr = m_vertLabeler-> format( m_cursor1value);
            pwset( m_pwPrefix + "cursor1/val", valStr);
            pwset( m_pwPrefix + "cursor1/mpos", cursor1mousePos);
            pwset( m_pwPrefix + "cursor1/mval", cursor1valueMousePos);
        }

        pwset( m_pwPrefix + "plotStyle", m_plotStyle);
        pwset( m_pwPrefix + "nGauss", m_g1dInput.nGaussians);
        pwset( m_pwPrefix + "nPoly", m_g1dInput.poly);
        pwset( m_pwPrefix + "mean", m_vertLabeler-> format( mean));
        pwset( m_pwPrefix + "rms", m_vertLabeler-> format( rms));
        pwsetdc( m_pwPrefix + "dc", 1);
    }

    //    dbg(1) << "... RedrawAllNow() done in " << debugTimer.elapsed() / 1000.0 << "s\n";
}

void Gauss1DFitterController::setFitParameters(int x1, int x2, int ng, int poly)
{
    // if there is no data, or the fitter has not been set, return
    if( m_data.size() == 0 || m_g1dfitManager == 0) return;

    // make sure number of initial guesses matches the requested number of gaussians
    if( int(m_initialGuess.size()) > ng)
        m_initialGuess.resize( ng);
    while( int(m_initialGuess.size()) < ng && m_initialGuess.size() > 0) {
        Optimization::Gauss1dNiceParams nice;
        nice.amplitude = 0.75 * m_zoomY1 + 0.25 * m_zoomY2;
        nice.center = drand48() * (x2 - x1) + x1;
        nice.fwhm = 0.5 * double(x2-x1) * 0.9 /(ng+1);
        m_initialGuess.push_back( nice);
    }

    // set up the initial guess vector
    if( m_initialGuess.size() > 0) {
        int nparams = ng * 3 + poly;
        m_g1dInput.initGuess = std::vector<double>( nparams, 0.0);
        for( size_t i = 0 ; i < m_initialGuess.size() ; i ++ ) {
            std::vector<double> ugly = m_initialGuess[i].convertToUgly();
            m_g1dInput.initGuess[i*3+0] = ugly[0];
            m_g1dInput.initGuess[i*3+1] = ugly[1];
            m_g1dInput.initGuess[i*3+2] = ugly[2];
        }
    }
    else {
        m_g1dInput.initGuess.clear();
    }

    m_g1dInput.left = clamp<int>(x1, 0, int(m_data.size())-1);
    m_g1dInput.right = clamp<int>(x2, 0, int(m_data.size())-1);

    swap_ordered( m_g1dInput.left, m_g1dInput.right);

    m_g1dInput.data = m_data;
    m_g1dInput.nGaussians = ng;
    m_g1dInput.poly = poly;
    m_g1dInput.isNull = false;
    if( ng + poly == 0) m_g1dInput.isNull = true;
    if( m_data.size() == 0) m_g1dInput.isNull = true;

    //    m_g1dInput.nGaussians = 1;
    //    m_g1dInput.poly = 1;
    m_g1dfitManager->request( m_g1dInput);

    markDirty();
}

void Gauss1DFitterController::setManualMode(bool val)
{
    if( val) {
        makeEvenlyDistributedInitialGuess();
    }
    else {
        // if we are turning manual mode off, clear the initial guess and restart
        // the fitting without it
        m_initialGuess.clear();
    }
    // restart the guessing with whatever we put into the initial guess
    setFitParameters( m_g1dInput.left, m_g1dInput.right, m_g1dInput.nGaussians, m_g1dInput.poly);
    markDirty();
}

//void Gauss1DFitterController::showManipulationBars(bool val)
//{
//    m_showManipulationBars = val;
//    markDirty();
//}

void Gauss1DFitterController::enableRandomHeuristics( bool val)
{
    m_g1dInput.randomHeuristicsEnabled = val;
    // restart the guessing with whatever we put into the initial guess
    setFitParameters( m_g1dInput.left, m_g1dInput.right, m_g1dInput.nGaussians, m_g1dInput.poly);
    markDirty();
}

void Gauss1DFitterController::makeEvenlyDistributedInitialGuess()
{
    m_initialGuess.clear();
    if( ! m_vars.manualInitialConditions-> get()) return;

    int left = m_g1dResults.input.left;
    int right = m_g1dResults.input.right;
    int ng = m_g1dResults.input.nGaussians;
    double h = m_y1 + 10 + m_buffer.height() * 0.25;
    //    for( int x = left ; x < right ; x ++) {
    //        if( x >= 0 && x < m_data.size())
    //            if( std::isnan(h) || m_data[x] > h) h = m_data[x];
    //    }

    for( int i = 0 ; i < ng ; i ++ ) {
        Optimization::Gauss1dNiceParams nicep;
        nicep.center = left + double(right-left)/(ng+1) * (i+1);
        nicep.fwhm = 0.5 * double(right-left) * 0.9 /(ng+1);
        nicep.amplitude = tty1inv(h);
        h += 10;
        m_initialGuess.push_back( nicep);
    }

    markDirty();
}

void Gauss1DFitterController::markDirty(Dirty dirty)
{
    switch (dirty) {
    case EVERYTHING:
        m_dirtyFlags.setAll();
        break;
    case CURSOR1:
        m_dirtyFlags.cursor1 = true;
        break;
    default:
        break;
    }

    if( ! m_visibleOnClient) {
        if(m_updateTimer->isActive())
            m_updateTimer->stop();
        return;
    }

    //    if(! m_updateTimer->isActive())
    //        m_updateTimer->start(0);

    int delay = 0;
    if( m_data.size() < m_total) delay = 100;
    m_updateTimer->start(delay);
}

void Gauss1DFitterController::updateTimerCB()
{
    if( ! m_visibleOnClient) return;

    // update m_buffer with the painting
    redrawAllNow();

    if( m_dirtyFlags.graph && ! m_viewName.isEmpty()) {
        // TODO: deferred vs immediate - investigate which is better...
        GetStateManager().ViewManager().RenderViewDeferred( m_viewName.toStdString());
        //            GetStateManager().ViewManager().RenderViewImmediate( m_viewName.toStdString());
    }

    // reset all dirty flags
    m_dirtyFlags = DirtyFlags();
}

void Gauss1DFitterController::SetClientSize(Size clientSize)
{
    m_buffer = QImage( clientSize.Width, clientSize.Height, m_buffer.format());
    markDirty();
}


Size Gauss1DFitterController::GetActualSize()
{
    return CSI::PureWeb::Size(m_buffer.width(), m_buffer.height());
}

void Gauss1DFitterController::RenderView(RenderTarget image)
{
    if( ! m_visibleOnClient) return;

    dbg(1) << "RenderView profile " << m_myId;
    ByteArray bits = image.RenderTargetImage().ImageBytes();
    ByteArray::Copy(m_buffer.scanLine(0), bits, 0, bits.Count());

    //    // attach extra info to the frame
    //    CSI::Collections::Map<String,String> map = image.Parameters();
    //    map["xxyyzz"] = QString("%1 %2").arg(this->m_myId).arg(this->m_zoomY1).toStdString();
}

void Gauss1DFitterController::PostMouseEvent(const Ui::PureWebMouseEventArgs & /*mouseEvent*/)
{
}

QString Gauss1DFitterController::formatAmplitudeWCS(double val)
{
    return m_vertLabeler-> format( val);
}

QString Gauss1DFitterController::formatPositionWCS(double pos)
{
    return m_horizLabeler-> format( pos);
}

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

QString Gauss1DFitterController::formatFwhmWCS(double fwhm, double pos)
{
    auto dist = horizontalLabeler()-> distance( pos - fwhm/2.0, pos + fwhm/2.0);
    QString formattedDistance = dist.v2.value.plain + dist.v2.unit.plain;
    dbg(1) << "XXXX formatted distance = " << formattedDistance;

    return formattedDistance;


//    QString unformattedAnswer = QString::number(fwhm) + " pix";
//    std::vector< WcsHelper::LabelAndUnit > lus = m_wcsHelper.getLabels();

//    if( m_formatterType == "Ix" && lus.size() > 1) {
//        std::vector<double> c1 = m_wcsHelper.toWorld( pos - fwhm/2, m_dataCursor[1], 0);
//        std::vector<double> c2 = m_wcsHelper.toWorld( pos + fwhm/2, m_dataCursor[1], 0);
//        double d = calculateAngle( c1[0], c1[1], c2[0], c2[1]);
//        return formatAngle( d) + " or " + formattedDistance;
//    }
//    if( m_formatterType == "Ix" && lus.empty()) return unformattedAnswer;
//    if( m_formatterType == "Iy" && lus.size() > 1) {
//        std::vector<double> c1 = m_wcsHelper.toWorld( m_dataCursor[0], pos - fwhm/2, 0);
//        std::vector<double> c2 = m_wcsHelper.toWorld( m_dataCursor[0], pos + fwhm/2, 0);
//        double d = calculateAngle( c1[0], c1[1], c2[0], c2[1]);
//        return formatAngle( d) + " or " + formattedDistance;
//    }
//    if( m_formatterType == "Iy" && lus.empty()) return unformattedAnswer;

//    std::vector<double> c1 = m_wcsHelper.toWorld( 0, 0, pos - fwhm/2);
//    std::vector<double> c2 = m_wcsHelper.toWorld( 0, 0, pos + fwhm/2);

//    QString unit = "";
//    if( lus.size() > 2 && ! lus[2].unitHtml.isEmpty())
//        unit = " " + lus[2].unitHtml;

//    return QString::number( fabs(c2[2] - c1[2])) + unit +  + " or " + formattedDistance;
}

//QString
//Gauss1DFitterController::gaussiansToTable(
//        std::vector<Optimization::Gauss1dNiceParams> & gs,
//        QString heading
//        )
//{
//    TextTable ttable;
//    ttable.set( 0, 0, heading);
//    ttable.set( 0, 1, "position");
//    ttable.set( 0, 2, "amplitude");
//    ttable.set( 0, 3, "fwhm");

//    ttable.setJustification( 0, TextTable::Left);
//    ttable.setJustification( 1, TextTable::Right);
//    ttable.setJustification( 2, TextTable::Right);
//    ttable.setJustification( 3, TextTable::Right);

//    for( size_t i = 0 ; i < gs.size() ; i ++ ) {
//        Optimization::Gauss1dNiceParams & nice = gs[i];
//        QString sep = (gs.size() < 10 || (i+1) > 9) ? " " : "  ";
//        QString col1 = QString("Gaussian%1#%2:")
//                .arg(sep).arg(i+1);
//        ttable.set( i+1, 0, col1);
//        QString col2, col3, col4;
//        if( m_vars.useWCS-> get()) {
//            col2 = formatPositionWCS( nice.center);
//            col3 = formatAmplitudeWCS( nice.amplitude);
//            col4 = formatFwhmWCS( nice.fwhm, nice.center);
//        }
//        else {
//            col2 = QString::number( nice.center);
//            col3 = QString::number( nice.amplitude);
//            col4 = QString::number( nice.fwhm);
//        }
//        ttable.set( i + 1, 1, col2);
//        ttable.set( i + 1, 2, col3);
//        ttable.set( i + 1, 3, col4);
//    }
//    return ttable.toString();
//}


QString
Gauss1DFitterController::gaussiansToTable2(
        std::vector<Optimization::Gauss1dNiceParams> & gs,
        std::vector<Optimization::Gauss1dNiceParams> & gs2,
        QString heading
        )
{
    TextTable ttable;
    ttable.set( 0, 0, heading);
    ttable.set( 0, 1, "position");
    ttable.set( 0, 2, "amplitude");
    ttable.set( 0, 3, "fwhm");
    ttable.set( 0, 4, "ini-pos.");
    ttable.set( 0, 5, "ini-ampl");
    ttable.set( 0, 6, "ini-fwhm");

    ttable.setJustification( 0, TextTable::Left);
    ttable.setJustification( 1, TextTable::Right);
    ttable.setJustification( 2, TextTable::Right);
    ttable.setJustification( 3, TextTable::Right);
    ttable.setJustification( 4, TextTable::Right);
    ttable.setJustification( 5, TextTable::Right);
    ttable.setJustification( 6, TextTable::Right);

    for( size_t i = 0 ; i < gs.size() ; i ++ ) {
        Optimization::Gauss1dNiceParams & nice = gs[i];
        QString sep = (gs.size() < 10 || (i+1) > 9) ? " " : "  ";
        QString col1 = QString("Gaussian%1#%2:")
                .arg(sep).arg(i+1);
        ttable.set( i+1, 0, col1);
        QString col2, col3, col4;
        if( m_vars.useWCS-> get()) {
            col2 = formatPositionWCS( nice.center);
            col3 = formatAmplitudeWCS( nice.amplitude);
            col4 = formatFwhmWCS( nice.fwhm, nice.center);
        }
        else {
            col2 = QString::number( nice.center);
            col3 = QString::number( nice.amplitude);
            col4 = QString::number( nice.fwhm);
        }
        ttable.set( i + 1, 1, col2);
        ttable.set( i + 1, 2, col3);
        ttable.set( i + 1, 3, col4);

        if( i >= gs2.size()) continue;
        {
            Optimization::Gauss1dNiceParams & nice = gs2[i];
            QString sep = (gs.size() < 10 || (i+1) > 9) ? " " : "  ";
            QString col2, col3, col4;
            if( m_vars.useWCS-> get()) {
                col2 = formatPositionWCS( nice.center);
                col3 = formatAmplitudeWCS( nice.amplitude);
                col4 = formatFwhmWCS( nice.fwhm, nice.center);
            }
            else {
                col2 = QString::number( nice.center);
                col3 = QString::number( nice.amplitude);
                col4 = QString::number( nice.fwhm);
            }
            ttable.set( i + 1, 4, col2);
            ttable.set( i + 1, 5, col3);
            ttable.set( i + 1, 6, col4);
        }
    }
    return ttable.toString();
}


double Gauss1DFitterController::measureLabelX(const QString &str)
{
    return m_labelFontMetrics-> width( str);
}

double Gauss1DFitterController::measureLabelY(const QString & /*str*/)
{
    return m_labelFontMetrics-> height();
}


void Gauss1DFitterController::setCursor1(qint64 pos)
{
    if( m_cursor1 == pos) return;
    m_cursor1 = pos;

    if( ! m_visibleOnClient) return;


//    markDirty( CURSOR1);

    // TODO: make this part of markDiry optimization, i.e. markDirty( CURSOR1);
    // because right now we are just duplicating code here!!!!

    // find out the value at the cursor
    double cursor1mousePos = ttx1( m_cursor1);
    double cursor1valueMousePos = 0.0;
    if( m_cursor1 >= m_i1 && m_cursor1 <= m_i2 && m_cursor1 >= 0 && m_cursor1 < qint64(m_data.size())) {
        m_cursor1value = m_data[ m_cursor1];
        cursor1valueMousePos = tty1( m_cursor1value);
    } else {
        m_cursor1value = INFINITY;
        cursor1valueMousePos = INFINITY;
    }

    if( m_cursor1 < 0) {
        pwset( m_pwPrefix + "cursor1/pos", "n/a");
        pwset( m_pwPrefix + "cursor1/val", "n/a");
        pwset( m_pwPrefix + "cursor1/mpos", "");
        pwset( m_pwPrefix + "cursor1/mval", "");
    }
    else {
//        pwset( m_pwPrefix + "cursor1/pos", m_cursor1 + 1);
        pwset( m_pwPrefix + "cursor1/pos", m_horizLabeler-> format( m_cursor1));
        QString valStr = m_vertLabeler-> format( m_cursor1value);
//        QString valStr;
//        if( m_vars.useWCS-> get())
//            valStr = m_wcsHelper.formatValue( m_cursor1value);
//        else
//            valStr = QString::number( m_cursor1value);
        pwset( m_pwPrefix + "cursor1/val", valStr);
        pwset( m_pwPrefix + "cursor1/mpos", cursor1mousePos);
        pwset( m_pwPrefix + "cursor1/mval", cursor1valueMousePos);
    }

}

void Gauss1DFitterController::setCursor1mouse(qint64 pos)
{
//    if( pos >= 0)
//        m_cursor1 = round(ttx1inv( pos));
//    else
//        m_cursor1 = -1;
//    markDirty( CURSOR1);

    if( pos >= 0) {
        setCursor1( round(ttx1inv( pos)));
    }
    else {
        setCursor1( -1);
    }
}

//void Gauss1DFitterController::setShowCursor1(bool val)
//{
//    m_showCursor1 = val;
//    markDirty();
//}

//bool Gauss1DFitterController::getShowCursor1()
//{
//    return m_showCursor1;
//}

qint64 Gauss1DFitterController::getCursor1() const
{
    if( m_cursor1 < 0) return -1;
    return m_cursor1;
}

void Gauss1DFitterController::setXZoom(quint64 min, quint64 max)
{
    m_zoomX1 = clamp<int>( min, 0, m_total-1);
    m_zoomX2 = clamp<int>( max, 0, m_total-1);
    setAutoX( false);
    markDirty();
}

void Gauss1DFitterController::setYZoom(double min, double max)
{
    m_zoomY1 = min;
    m_zoomY2 = max;
    setAutoY( false);
    markDirty();
}

void Gauss1DFitterController::setXZoomLimits(quint64 min, quint64 max)
{
    m_zoomXmin = min;
    m_zoomXmax = max;
    markDirty();
}

void Gauss1DFitterController::setYZoomAutoRange(double min, double max)
{
    m_autoYmin = min;
    m_autoYmax = max;
    markDirty();
}

void Gauss1DFitterController::globalVariableCB(const QString &name)
{
    // guard internals until constructor is finished
    if( m_ignoreVarCallbacks) return;

    dbg(1) << "g1d gvar " << name << " changed.";

    if( name == "initialGuess") {
        QString val = m_vars.initialGuess-> get();
        dbg(1) << "initGuess = " << val;

        QStringList lst = val.split(' ', QString::SkipEmptyParts);

        // construct a new intial guess from the received parameters
        std::vector<Optimization::Gauss1dNiceParams> newInitialGuess;
        int ng = lst.size() / 3;
        bool allGood = true;
        newInitialGuess.clear();
        for( int i = 0 ; i < ng ; i ++ ) {
            Optimization::Gauss1dNiceParams nice;
            bool ok;
            double cx = lst[i*3+0].toDouble( & ok);
            if( ! ok) { allGood = false; break; }
            double dx = lst[i*3+1].toDouble( & ok);
            if( ! ok) { allGood = false; break; }
            double ay = lst[i*3+2].toDouble( & ok);
            if( ! ok) { allGood = false; break; }
            if( ! std::isfinite(cx)) { allGood = false; break; }
            if( ! std::isfinite(dx)) { allGood = false; break; }
            if( ! std::isfinite(ay)) { allGood = false; break; }

            nice.center = ttx1inv( cx);
            nice.fwhm = 2 * (ttx1inv( cx + dx) - ttx1inv( cx));
            nice.amplitude = tty1inv( ay);

            newInitialGuess.push_back( nice);
        }
        if( ! allGood) newInitialGuess.clear();

        // compare the new initial guess with the old one (this is 'necessary' because
        // server sometimes sets the state due to window resize, in which case the
        // initial guess should not really change at all)
        double diffSq = 0.0;
        const double diffSqThreshold = 1e-3;
        auto dsq = [&]( const Optimization::Gauss1dNiceParams & n1, const Optimization::Gauss1dNiceParams & n2) {
            auto sq = []( const double & x) { return x*x; };
            return sq( ttx1(n1.center) - ttx1(n2.center))
                    + sq( ttx1(n1.fwhm) - ttx1(n2.fwhm))
                    + sq( tty1(n1.amplitude) - tty1(n2.amplitude));
        };
        if( newInitialGuess.size() != m_initialGuess.size()) {
            diffSq = diffSqThreshold;
        }
        else {
            for( size_t i = 0 ; i < newInitialGuess.size() ; i ++ ) {
                diffSq += dsq( newInitialGuess[i], m_initialGuess[i]);
            }
        }
        if( diffSq < diffSqThreshold) {
            dbg(1) << "change not big enough to recalculate..." << diffSq;
        }
        else {
            dbg(1) << "recalculating because " << diffSq << " >= " << diffSqThreshold;
            // restart the fit with the initial guess
            m_initialGuess = newInitialGuess;
            setFitParameters( m_g1dInput.left, m_g1dInput.right, m_g1dInput.nGaussians, m_g1dInput.poly);
        }
    }
    else if( name == "manualInitialConditions") {
        setManualMode( m_vars.manualInitialConditions-> get());
    }
    else if( name == "randomHeuristics") {
        enableRandomHeuristics( m_vars.randomHeuristics-> get());
    }
    else if( name == "autoX") {
        bool val = m_vars.autoX-> get();
        emit autoXChanged( val);
        markDirty();
    }
    else if( name == "autoY") {
        bool val = m_vars.autoY-> get();
        emit autoYChanged( val);
        markDirty();
    }

    markDirty();

}

void Gauss1DFitterController::updateWcs()
{
    markDirty();
}

//void Gauss1DFitterController::setXCaption(const QString &str)
//{
//    m_horizLabeler-> setCaption1( str);
//    markDirty();
//}

//void Gauss1DFitterController::redraw()
//{
//    markDirty( EVERYTHING);
//}
