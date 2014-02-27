#include "FitsViewerServer.h"

#include <cmath>
#include <climits>

#include <QImage>
#include <QPainter>
#include <QFont>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QSettings>

#include "FitsParser.h"
#include "common.h"
#include "RaiCacherService.h"
#include "WcsHelper.h"


FitsViewerServer::FitsViewerServer(QObject *parent) :
    QObject(parent)
{
    dbg(1) << "FitsViewerServer constructor\n";

    currentFrame_ = 0;
    needsReload_ = true;
    // resize( QSize(960,640));
    drawOrigin_ = QPointF(0,0);
    drawWidth_ = 1.0;
    histMin_ = 0.0;
    histMax_ = 1.0;
    cmap_ = ColormapFunction::heat();
    hcMap_ = HistogramColormapFunctor( histMin_, histMax_, cmap_);
    m_cachedHcMap = CachedRgbFunction( hcMap_, histMin_, histMax_, 10000, Rgb(255,0,0));
    m_wcsHelper = new WcsHelper;

    m_cc = QSharedPointer< RaiLib::Cache::Controller > (
                new RaiLib::Cache::Controller(
                    RaiLib::Cache::Controller::InitParameters::fromIniFile (
                        QDir::homePath () + "/.sfviewer.config", "cache")
                    ));
    QSettings s( QDir::homePath () + "/.sfviewer.config", QSettings::IniFormat);
    s.beginGroup ( "cacherService");
    qint64 maxMem = s.value("maxMemory", "500000000").toLongLong ();
    dbg(1) << "maxMem = " << maxMem << "\n";

    m_cacherService = QSharedPointer< RaiLib::Cache::RaiCacherService> (
                new RaiLib::Cache::RaiCacherService(
                m_cc-> getInitParameters (),
                maxMem
                ));
}

FitsViewerServer::~FitsViewerServer ()
{
    if( m_wcsHelper) {
        delete m_wcsHelper;
    }
}


// anonymous namespace to hide fits2image()

namespace {
// converts a fits frame to qimage, but only the 'rect' part, using RgbFunctor to
// do the conversion. RgbFunctor has to supply operator() which takes a double
// and returns Rgb. It must handle all possible values, including NaNs.
template <typename RgbFunctor>
//static
QImage fits2image( FitsParser & parser, int frame, RgbFunctor & rgbFunctor)
{
    QTime timer; timer.restart();

    const FitsParser::HeaderInfo & headerInfo = parser.getHeaderInfo ();
    if( frame < 0 || frame >= headerInfo.totalFrames )
        return QImage();
    int width = headerInfo.naxis1;
    int height = headerInfo.naxis2;

    static std::vector < QRgb > outBuffer;
    //    if( int(outBuffer.size()) != width * height)
    outBuffer.resize( width * height);

    // make sure this frame is cached, to make src() faster
    const std::vector<double> & src = parser.cacheFrame ( frame);

#pragma omp parallel for
    for( int y = 0 ; y < height ; y ++ ) {
        QRgb * outPtr = & (outBuffer[(height-y-1) * width]);
        const double * srcPtr = & src[ y * width];
        for( int x = 0 ; x < width ; x ++ ) {
            * outPtr = rgbFunctor.qrgb( * srcPtr );
            outPtr ++;
            srcPtr ++;
        }
    }

    dbg(1) << "fits2image " << frame << " completed in " << timer.elapsed() / 1000.0 << "s\n"; timer.restart();

    QImage img( reinterpret_cast<uchar *>( & outBuffer[0]),
            width, height, QImage::Format_ARGB32);

    return img;
}

} // anonymous namespace

bool FitsViewerServer::loadIfNeeded()
{
    if( needsReload_) {
        QTime t; t.restart ();
        fullFrameImage_ = fits2image( parser(), currentFrame (), m_cachedHcMap );
        dbg(1) << "frame recompute in " << t.elapsed () / 1000.0 << "s\n";
        needsReload_ = false;
    }
    return ! fullFrameImage_.isNull();
}

bool
FitsViewerServer::loadFitsFile(
    const FitsFileLocation & fLocation)
{
    dbg(1) << ConsoleColors::bright()
           << "FitsViewerServer::loadFitsFile: " << fLocation.getLocalFname()
           << ConsoleColors::reset()
           << "\n";
    if( fLocation == fLocation_) return true;

    fLocation_ = fLocation;
    currentFrame_ = 0;
    needsReload_ = true;
    fullFrameImage_ = QImage();
    m_cireader = QSharedPointer< CachedImageReader > (
                new CachedImageReader( m_cc, fLocation_));    

    // tell wcshelper we switched the file
    if( ! m_wcsHelper-> openFile ( fLocation.getLocalFname ())) {
        warn() << "WCS helper failed to open file " << fLocation.getLocalFname();
    }

    // update wcs hero
    m_wcsHero.reset( WcsHero::Hero::constructFromFitsFile( fLocation.getLocalFname()));

    // update the cache if it's not updated yet
    m_cacherService-> doNow ( fLocation);

    return m_cireader-> valid ();
}

bool FitsViewerServer::resize(const QSize & size)
{
    dbg(1) << "FitsViewerServer::resize " << size.width() << "x"
           << size.height() << "\n";
    renderBuffer_ = QImage( size, QImage::Format_RGB32);
    return true;
}

void FitsViewerServer::setDrawOrigin( QPointF pt)
{
    drawOrigin_ = pt;
}

void
FitsViewerServer::getCentered (QPointF & origin, double & width)
{
    // two cases, image's w/h is higher than window, or opposite
    double ww = renderBuffer_.width ();
    double wh = renderBuffer_.height ();
    double iw = parser().getHeaderInfo ().naxis1;
    double ih = parser().getHeaderInfo ().naxis2;
    if( ww * ih > wh * iw)
    {
        width = wh * iw / ww / ih;
        origin = QPointF( (ww - width * ww)/2,0);
    } else {
        width = 1.0;
        origin = QPointF(0,(wh - ih * width * ww / iw)/2);
    }
}

void FitsViewerServer::setDrawWidth(double width)
{
    drawWidth_ = width;
}

void
FitsViewerServer::setHistogram (
    double min,
    double max)
{
    histMin_ = min;
    histMax_ = max;
    hcMap_ = HistogramColormapFunctor( histMin_, histMax_, cmap_);
    m_cachedHcMap = CachedRgbFunction( hcMap_, histMin_, histMax_, 10000, Rgb(0,0,0));
    emit cachedColormapChanged();
    needsReload_ = true;
    fullFrameImage_ = QImage();
}

void
FitsViewerServer::setColormap (
    ColormapFunction cmap)
{
    cmap_ = cmap;
    hcMap_ = HistogramColormapFunctor( histMin_, histMax_, cmap_);
    m_cachedHcMap = CachedRgbFunction( hcMap_, histMin_, histMax_, 10000, Rgb(0,0,0));
    emit cachedColormapChanged();
    needsReload_ = true;
    fullFrameImage_ = QImage();
}

bool FitsViewerServer::loadFrame(int frame)
{
    currentFrame_ = frame;
    needsReload_ = true;
    parser().cacheFrame ( frame);
    fullFrameImage_ = QImage();
//    loadIfNeeded();
    return true;
}

double FitsViewerServer::getValue (int x, int y, int z)
{
    int iw = parser().getHeaderInfo().naxis1;
    int ih = parser().getHeaderInfo().naxis2;
    int id = parser().getHeaderInfo ().totalFrames;
    if( x < 0 || x >= iw || y < 0 || y >= ih || z < 0 || z >= id)
        return std::numeric_limits<double>::quiet_NaN();
    return parser().src ( x, y, z);
}

const CachedRgbFunction &FitsViewerServer::getCachedColormap() const
{
    return m_cachedHcMap;
}

/// format a value together with units
QString
FitsViewerServer::formatValue (double val)
{
    QString res = QString::number( val);

    // append units
    if( m_wcsHero) {
        QString units = m_wcsHero-> htmlBunit();
        if( ! units.isEmpty()) {
            res = res + " " + units;
        }
    }

    return res;
}

// format a cursor position based on the infromation from casa
// (the first pixel's left/bottom most point coordinate is 0,0)
QStringList
FitsViewerServer::formatCursor (double x, double y, double frame, bool withLabels)
{
    return m_wcsHelper-> formatCursor ( x, y, frame, withLabels);
}

QString
FitsViewerServer::getWCS()
{
    return m_wcsHelper-> getWCS();
}

void
FitsViewerServer::setWCS(const QString & s)
{
    m_wcsHelper-> setWCS( s);

    WcsHero::Hero::SKYCS skycs = WcsHero::Hero::string2skycs( s);
    m_wcsHero-> setSkyCS( skycs);
}

QString FitsViewerServer::formatCoordinate(double x, double y)
{
    QStringList fmt = m_wcsHelper-> formatCursor( x, y, currentFrame(), true);
    if( fmt.size() >= 2) {
        return fmt[0] + " " + fmt[1];
    }
    else {
        return "";
    }
//    WcsHelper::FormattedCoordinate fc = m_wcsHelper-> formatCoordinate(
//                x, y);
//    if( fc.raValue.isEmpty()) return "";
//    return fc.raLabel + ": " + fc.raValue + " "
//            + fc.decLabel + ": " + fc.decValue;
}

void FitsViewerServer::queueForCacher(const FitsFileLocation &floc, bool now)
{
    if( m_cacherService.isNull ()) return;

    if( now)
        m_cacherService-> doNow ( floc);
    else
        m_cacherService-> doLater ( floc);
}

QString FitsViewerServer::valueUnit()
{
    return m_wcsHelper-> valueUnit();
}

QString FitsViewerServer::totalFluxDensityUnit()
{
    return m_wcsHelper-> totalFluxDensityUnit();
}

WcsHero::Hero::SharedPtr FitsViewerServer::wcsHero()
{
    return m_wcsHero;
}


const QImage & FitsViewerServer::renderCurrentFrame()
{
    loadIfNeeded();

    // now render the frame into the image
    renderBuffer_.fill( QColor("black").rgb());
    QPainter p( & renderBuffer_);
    double x1 = drawOrigin_.x();
    double dw = renderBuffer_.width() * drawWidth_;
    double y1 = drawOrigin_.y();
    double dh = fullFrameImage_.height() * dw / fullFrameImage_.width();

    bool pixelZoom = fullFrameImage_.width() > dw;
    p.setRenderHint ( QPainter::SmoothPixmapTransform,
                      pixelZoom);
    p.drawImage( QRectF( x1, y1, dw, dh), fullFrameImage_);

    return renderBuffer_;
}

QStringList FitsViewerServer::getFileList( const QString & dirName)
{
    QDir dir( dirName);
    QList <QFileInfo> flist = dir.entryInfoList( QStringList("*.fits"), QDir::Files);

    QStringList res;
    for( int i = 0 ; i < flist.size() ; i ++) {
        res.push_back( QString("%1 %2").arg(flist[i].size()).arg(flist[i].fileName()));
    }
    return res;
}

const FitsParser::HeaderInfo & FitsViewerServer::getHeaderInfo()
{
    return parser().getHeaderInfo();
}

RaiLib::HistogramInfo FitsViewerServer::getFrameInfo(
    int frame)
{
    return m_cireader-> getFrameInfo ( frame);
//    return parser().getFrameInfo (frame);
}


RaiLib::HistogramInfo FitsViewerServer::getCurrentFrameInfo()
{
    return getFrameInfo (currentFrame ());
}

/*
// returns a profile along X axis (z is a frame)
const QList<double> &
FitsViewerServer::getProfileX( int x, int y, int z)
{
    return parser().getProfileX ( x, y, z);
}

// returns a profile along Y axis (z is a frame)
const QList<double> &
FitsViewerServer::getProfileY( int x, int y, int z)
{
    return parser().getProfileY ( x, y, z);
}

// returns a profile along Z axis (z is a frame)
const QList<double> &
FitsViewerServer::getProfileZ( int x, int y, int z)
{
    return parser().getProfileZ ( x, y, z);
}

*/
