#include <QImage>
#include <QPainter>
#include <QMouseEvent>

#include "CanvasWidget.h"
#include "HistogramDialog.h"
#include "FitsViewerLib/IGraphLabeler.h"
#include <functional>

HistogramDialog::HistogramDialog(QWidget * parent, FitsViewerServer & fvs) :
    QObject(parent),
    m_fvs( fvs)
{
    m_frameInfoNumber = -1;

//    m_ui->setupUi(this);

    // connect signals
    connect ( & m_recalcTimer, SIGNAL(timeout()), SLOT(recalcTimerCB()));

    bgColor_ = QColor("black");
    markerColor_ = QColor("white");
    rectColor_ = QColor( 255, 255, 255, 64);
    zMin_ = 0; zMax_ = 0;
    gMin_ = 0; gMax_ = 0;
    min_ = 0; max_ = 0;
    m_imageWidth = m_imageHeight = 100;
    m_smoothGraph = true;
    m_logScale = true;
    m_cursorX = 0;
    m_marginLeft = 5;
    m_marginRight = 5;
    m_marginTop = 12;
    m_marginBottom = 20;
    m_visibleOnClient = false;

    // setup a recalculation timer
    m_recalcTimer.setSingleShot(true);
    recalculateHistogramDelayed();

    m_buffer = QImage( QSize(100,100), QImage::Format_ARGB32_Premultiplied);

    // PureWeb
    // the default preset is 99.5%
    pwset( "/Histogram/Index", 3);

    GetStateManager().CommandManager().AddUiHandler(
                "histogram",
                CSI::Bind(this, &HistogramDialog::pwCommandHandler));
    CSI::PureWeb::Server::ViewImageFormat viewImageFormat;
    viewImageFormat.PixelFormat = CSI::PureWeb::PixelFormat::Bgrx32;
    viewImageFormat.ScanLineOrder = CSI::PureWeb::ScanLineOrder::TopDown;
    viewImageFormat.Alignment = 4;
    GetStateManager().ViewManager().RegisterView("HistogramView", this);
    GetStateManager().ViewManager().SetViewImageFormat("HistogramView", viewImageFormat);

    // create and bind global variables
    // ================================

    m_ignoreVarCallbacks = true;
    QString m_pwPrefix = "/Histogram/";
    binder( m_vars.cursor, m_pwPrefix, "cursor");
    binder( m_vars.lowerClip, m_pwPrefix, "LowerClip");
    binder( m_vars.upperClip, m_pwPrefix, "UpperClip");
    m_ignoreVarCallbacks = false;

}

HistogramDialog::~HistogramDialog()
{
}

void HistogramDialog::activatePreset (int id, bool zoom)
{
    id = clamp( id, 0, 6);
    switch( id) {
    case 0: min_ = frameInfo_.min95;   max_ = frameInfo_.max95; break;
    case 1: min_ = frameInfo_.min98;   max_ = frameInfo_.max98; break;
    case 2: min_ = frameInfo_.min99;   max_ = frameInfo_.max99; break;
    case 3: min_ = frameInfo_.min995;  max_ = frameInfo_.max995; break;
    case 4: min_ = frameInfo_.min999;  max_ = frameInfo_.max999; break;
    case 5: min_ = frameInfo_.min9999; max_ = frameInfo_.max9999; break;
    case 6: min_ = frameInfo_.min; max_ = frameInfo_.max; break;
    }

    if( zoom) {
        zoomToSelection();
    }
    recalculateHistogramDelayed();
    pwset ("/Histogram/Index", id);
    emit valuesChanged ( min_, max_);
}

void HistogramDialog::SetClientSize(CSI::PureWeb::Size clientSize)
{
    m_imageWidth = clientSize.Width;
    m_imageHeight = clientSize.Height;
    recalculateHistogramDelayed();
}

CSI::PureWeb::Size HistogramDialog::GetActualSize()
{
    //    return CSI::PureWeb::Size( m_imageWidth, m_imageHeight);
    return CSI::PureWeb::Size( m_buffer.width(), m_buffer.height());
}

void HistogramDialog::RenderView(CSI::PureWeb::Server::RenderTarget image)
{
    if(! m_visibleOnClient) return;

//    dbg(1) << "RenderView()\n";
//    // send the buffer to pureweb
//    QImage buff888;
//    if( m_buffer.format() != QImage::Format_RGB888)
//        buff888 = m_buffer.convertToFormat( QImage::Format_RGB888);
//    else
//        buff888 = m_buffer;
//    CSI::ByteArray bits = image.RenderTargetImage().ImageBytes();
//    CSI::ByteArray::Copy(buff888.scanLine(0), bits, 0, bits.Count());

    // send the buffer to pureweb
    CSI::ByteArray bits = image.RenderTargetImage().ImageBytes();
    CSI::ByteArray::Copy(m_buffer.scanLine(0), bits, 0, bits.Count());

}

void
HistogramDialog::pwCommandHandler (
        CSI::Guid /*sessionid*/,
        CSI::Typeless command,
        CSI::Typeless /*responses*/)
{

    std::string cmd = command["/cmd"].As<std::string>();

    if( cmd == "setMarkers") {

        double x1 = command["/x1"].As<double>();
        double x2 = command["/x2"].As<double>();

        x1 = (x1 * m_imageWidth - m_marginLeft) / (m_imageWidth - m_marginLeft - m_marginRight);
        x2 = (x2 * m_imageWidth - m_marginLeft) / (m_imageWidth - m_marginLeft - m_marginRight);

        min_ = x1 * (zMax_ - zMin_) + zMin_;
        if( min_ < zMin_) min_ = zMin_;
        if( min_ > max_) min_ = max_;
        max_ = x2 * (zMax_ - zMin_) + zMin_;
        if( max_ > zMax_) max_ = zMax_;
        if( min_ > max_) max_ = min_;

        pwset ("/Histogram/Index", -1);
        recalculateHistogramDelayed ();
        //        canvasRepaint ();
        //        updateSliderState ();
        emit valuesChanged ( min_, max_);
    } else if( cmd == "show") {
//        show ();
        pwset("/Histogram/Visible", 1);
    } else if( cmd == "hide") {
//        hide ();
        pwset("/Histogram/Visible", 0);
    } else if( cmd == "preset") {
        int ind = command["/index"].As<int>();
        bool zoom = command["/zoom"].As<bool>();
        activatePreset ( ind, zoom);
//        updateSliderState ();
    } else if( cmd == "zoom") {
        zoomToSelection ();
    } else if( cmd == "unzoom") {
        showFullRange ();
    } else if( cmd == "resize") {
        m_imageWidth = command["/width"].As<int>();
        recalculateHistogramDelayed ();
    }
    else if( cmd == "logScale") {
        m_logScale = command["/val"].As<bool>();
        recalculateHistogramDelayed ();
    }
    else if( cmd == "smoothGraph") {
        m_smoothGraph = command["/val"].As<bool>();
        recalculateHistogramDelayed ();
    }
    else if( cmd == "setCursor") {
        m_cursorX = command["/x"].As<int>();
        recalculateHistogramDelayed ();
    }
    else if( cmd == "setVisible") {
        std::string val = command["/val"].As<std::string>();
        m_visibleOnClient = command["/val"].As<bool>();
        recalculateHistogramDelayed();
    }
    else {
        dbg(0) << "Unknown histogram command " << cmd;
    }

}

void HistogramDialog::globalVariableCB(const QString &name)
{
    // guard internals until constructor is finished
    if( m_ignoreVarCallbacks) return;

    if( name == "cursor") {
        m_cursorX = m_vars.cursor-> get();
        recalculateHistogramDelayed();
    }
    else if( name == "LowerClip") {
//        dbg(1) << "lower clip = " << m_vars.lowerClip-> get();
        auto val = m_vars.lowerClip-> get();
        if( ! std::isfinite( val)) val = min_;
        val = clamp( val, gMin_, gMax_);
        min_ = val;
        swap_ordered( min_, max_);
        recalculateHistogramDelayed();
        emit valuesChanged ( min_, max_);
    }
    else if( name == "UpperClip") {
        dbg(1) << "upper clip = " << m_vars.upperClip-> get();
        auto val = m_vars.upperClip-> get();
        if( ! std::isfinite( val)) val = max_;
        val = clamp( val, gMin_, gMax_);
        max_ = val;
        swap_ordered( min_, max_);
        recalculateHistogramDelayed();
        emit valuesChanged ( min_, max_);
    }
    else {
        warn() << "Unknown global var: " << name;
    }

}

void
HistogramDialog::recalcTimerCB ()
{
    if(! m_visibleOnClient) return;

    recalculateHistogramRaw ();
}

void
HistogramDialog::recalculateHistogramDelayed ()
{
    if(! m_visibleOnClient) return;

    if( !m_recalcTimer.isActive())
        m_recalcTimer.start(100);
}

void
HistogramDialog::recalculateHistogramRaw ()
{
    if(! m_visibleOnClient) return;

    ScopedDebug sd( "Recalculating histogram (slow)", 1);

    // drawable area width/height
    int daWidth = m_imageWidth - m_marginLeft - m_marginRight;
    int daHeight = m_imageHeight - m_marginTop - m_marginBottom;

    m_histInfo.min = zMin_;
    m_histInfo.max = zMax_;
    m_histInfo.nNans = 0;
    m_histInfo.maxBin = 0;
    int nBins;
    if( m_smoothGraph)
        nBins = std::max( daWidth , 10);
    else
        nBins = std::max( daWidth / 10, 10);

    m_histInfo.bins.resize ( nBins);
    for( int i = 0 ; i < nBins ; i ++ )
        m_histInfo.bins[i] = 0;

    quint64 trueCount = 0; // number of pixels in the clip range
    quint64 totalPixels = 0; // number of pixels that are not nans
    if( m_cir) {

        FitsParser & p = m_cir-> parser (); // alias

        FitsParser::HeaderInfo header = p.getHeaderInfo ();
//        totalPixels = header.naxis1 * header.naxis2;
        for( int y = 0 ; y < header.naxis2 ; y ++ ) {
            for( int x = 0 ; x < header.naxis1 ; x ++ ) {
                double v = p.src ( x, y, m_frameInfoNumber);
                if( isnan (v)) {
                    m_histInfo.nNans ++;
//                    totalPixels --;
                } else {
                    // TODO: centering in bins...?
                    int i = floor (nBins * (v - m_histInfo.min)/(m_histInfo.max - m_histInfo.min));
                    if( i >= 0 && i < nBins) {
                        m_histInfo.bins[i] ++;
                    }
                    if( v >= min() && v <= max()){
                        trueCount ++;
                    }
                }
            }
        }
        totalPixels = header.naxis1 * header.naxis2 - m_histInfo.nNans;

        // compute the highest count (so that we can scale this when drawing)
        for( int i = 0 ; i < m_histInfo.bins.size () ; i ++ ) {
            m_histInfo.maxBin = std::max( m_histInfo.maxBin, m_histInfo.bins[i]);
        }
    }

    // render the histogram
    QColor barColor( "#0071FD");
//    QColor backgroundColor( "#F3F3DA");
    QColor backgroundColor( "#ffffff");
    QColor highlightColor( "#FF0000");
    m_buffer = QImage( QSize(m_imageWidth, m_imageHeight), m_buffer.format());
    int ih = m_imageHeight;
    QPainter painter( & m_buffer);
    painter.setRenderHint( QPainter::Antialiasing, true);
    painter.setRenderHint( QPainter::HighQualityAntialiasing, true);
    painter.fillRect ( m_buffer.rect (), backgroundColor);

    // over which bin is the cursor?
    int cursorBin = m_cursorX - m_marginLeft;
    if( ! m_smoothGraph)
        cursorBin = floor( (m_cursorX - m_marginLeft) / double(daWidth) * nBins);
    cursorBin = clamp( cursorBin, 0, nBins - 1);

    double maxY = m_histInfo.maxBin;
    if( m_logScale) maxY = log( maxY + 1);
    double dx = double(daWidth) / nBins;
    for( int i = 0 ; i < m_histInfo.bins.size () ; i ++ ) {
        double y = m_histInfo.bins[i];
        if(y == 0) continue;
        if( m_logScale) y = log(y + 1);
        y = (daHeight-2) * y / double( maxY) + 2;
        //        painter.fillRect( QRectF( dx * i, ih, dx, -y ), barColor);
        QPoint p1( round( dx  * i) + m_marginLeft, ih - m_marginBottom);
        QPoint p2( round( dx  * (i+1)) - 1 + m_marginLeft, ih - m_marginBottom - y);

        if( i == cursorBin)
            painter.fillRect( QRect( p1, p2 ), highlightColor);
        else
            painter.fillRect( QRect( p1, p2 ), barColor);
    }

    // draw faint rectangles around bars (only if in bar mode)
    if( ! m_smoothGraph) {
        for( int i = 0 ; i < m_histInfo.bins.size () ; i ++ ) {
            double y = m_histInfo.bins[i];
            if( m_logScale) y = log(y + 1);
            y = daHeight * y / double( maxY);
            //        painter.fillRect( QRectF( dx * i, ih, dx, -y ), barColor);
            QPoint p1( round( dx  * i) + m_marginLeft, ih - m_marginBottom);
            QPoint p2( round( dx  * (i+1)) - 1 + m_marginLeft, ih - m_marginBottom - y);
            painter.setPen( QPen( QColor("black"), 0.1));
            painter.drawRect( QRect( p1, p2 ));
        }
    }

    // draw labels for horizontal
    {
        double yy = m_imageHeight - m_marginBottom + 1;
        Plot2dLabelers::BasicLabeler::SharedPtr horizLabeler = std::make_shared< Plot2dLabelers::BasicLabeler >();

        QFont m_fontHorizontalLabels( "Arial", 8);
        QFontMetrics m_fmHorizontalLabels( m_fontHorizontalLabels);
        int labelFontHeight = m_fmHorizontalLabels.height();
        auto measureTxt = [&m_fmHorizontalLabels]( const QString & str) -> double {
            return m_fmHorizontalLabels.width( str);
        };

        horizLabeler-> setZoom( zMin_, zMax_);
        horizLabeler-> setPixels( daWidth);
        horizLabeler-> setDual( false);
        auto labels = horizLabeler-> compute( measureTxt);

        painter.setFont( m_fontHorizontalLabels);
        painter.setPen( "black");
        QVector<QPointF> ticks;
        ticks.append( QPointF( m_marginLeft - 5, yy));
        ticks.append( QPointF( m_imageWidth - m_marginRight + 5, yy));
        for( auto & label : labels) {
            double xx = label.centerPix + m_marginLeft;
            painter.drawText( QRectF( xx, yy + labelFontHeight / 2.0 + 5, 1, 1),
                              Qt::AlignCenter | Qt::TextDontClip,
                              label.txt1
                              );
            ticks.append( QPointF( xx, m_imageHeight - m_marginBottom + 1));
            ticks.append( QPointF( xx, m_imageHeight - m_marginBottom + 5));
        }
        // tick marks
        painter.setPen( QPen( QBrush("blue"), 1));
        painter.drawLines( ticks);
    }

    // draw the markers
    double x1 = (min() - zMin_) / (zMax_ - zMin_) * daWidth + m_marginLeft;
    double x2 = (max() - zMin_) / (zMax_ - zMin_) * daWidth + m_marginLeft;
    painter.fillRect( QRectF( x1, 5, x2-x1, 5 ), QColor("#008800"));

    // draw the current cursor position
    double binCenter = (cursorBin + 0.5) / nBins * (zMax_ - zMin_) + zMin_;
    double cursorBinVal = m_histInfo.bins[ cursorBin];

    GetStateManager().ViewManager().RenderViewDeferred("HistogramView");

    // tell clients where the markers are
    pwset( "/Histogram/marker1", x1 / m_imageWidth);
    pwset( "/Histogram/marker2", x2 / m_imageWidth);

    // tell clients other stats
//    pwset("/Histogram/LowerClip", m_fvs.formatValue( min_));
//    pwset("/Histogram/UpperClip", m_fvs.formatValue( max_));
    m_vars.lowerClip-> set( min_);
    m_vars.upperClip-> set( max_);
    pwset("/Histogram/GlobalMin", m_fvs.formatValue( frameInfo_.min));
    pwset("/Histogram/GlobalMax", m_fvs.formatValue( frameInfo_.max));
    pwset( "/Histogram/nNaNs", m_histInfo.nNans);
    pwset( "/Histogram/binValue", m_fvs.formatValue( binCenter));
    pwset( "/Histogram/nValuesInBin", cursorBinVal);
    pwset( "/Histogram/trueCount", QString::number( 100 * double(trueCount) / totalPixels, 'g', 6) + "%");
}

// this is normally called when a new frame is loaded (either user switches to another
// frame, or even loads a new file)
// - store the frame info
// - set global min/max from frame info
// - if preset is currently checked, recalculate the min/max & zoom based on that preset
void
HistogramDialog::setFrameInfo (
        qint64 frameNo,
        const QSharedPointer<CachedImageReader> cir)
{
    m_cir = cir;
    frameInfo_ = cir-> getFrameInfo ( frameNo);
    m_frameInfoNumber = frameNo;

    gMin_ = frameInfo_.min;
    gMax_ = frameInfo_.max;

    if( zMin_ == 0 && zMax_ == 0) {
        zMin_ = gMin_;
        zMax_ = gMax_;
    }

    // if the current histogram clip values are nans, apply histogram setting
    if( isnan(min()) || isnan(max()))
    {
        activatePreset ( 4, true);
    }

    // calculate histogram
    recalculateHistogramDelayed();
}

void
HistogramDialog::zoomToSelection ()
{
    zMin_ = min_ - (max_ - min_) * 0.1;
    zMax_ = max_ + (max_ - min_) * 0.1;

    recalculateHistogramDelayed ();
}

void
HistogramDialog::showFullRange ()
{
    zMin_ = gMin_;
    zMax_ = gMax_;

    recalculateHistogramDelayed ();
}

