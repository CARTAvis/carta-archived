/**
 *
 **/

#include "ImageRenderService.h"
#include "CartaLib/LinearMap.h"
#include <QColor>
#include <QPainter>

namespace NdArray = Carta::Lib::NdArray;

// most optimal Qt format seems to be Format_ARGB32_Premultiplied
static constexpr QImage::Format OptimalQImageFormat = QImage::Format_ARGB32_Premultiplied;

// however!!!! there seems to be a bug in QT's rendering of premultiplied images...
// there goes significant chunk of my life!!!!!!!!!!!!!!! GRRRRRRRRR!!!!!!!!!
// QPainter::drawImage( QRectF ....) will really mess up with high zoom (scaling)
// if source & destination are both ARGB32_Premultiplied
/// \todo check if the bug is still there in Qt5.4+, it definitely is there in Qt5.3
static constexpr bool QtPremultipliedBugStillExists = true;

/// internal algorithm for converting an instance of image interface to qimage
/// using the pixel pipeline
///
/// \tparam Pipeline
/// \param m_rawView
/// \param pipe
/// \param m_qImage
template < class Pipeline >
static void
iView2qImage( NdArray::RawViewInterface * rawView, Pipeline & pipe, QImage & qImage,
        QRgb nanColor)
{
    //qDebug() << "rv2qi2" << rawView-> dims();
    typedef double Scalar;

    QSize size( rawView->dims()[0], rawView->dims()[1] );

    QImage::Format desiredFormat = OptimalQImageFormat;
    if ( QtPremultipliedBugStillExists ) {
        desiredFormat = QImage::Format_ARGB32;
    }

    // QImage::Format desiredFormat = QImage::Format_ARGB32;
    if ( qImage.format() != desiredFormat ||
         qImage.size() != size ) {
        qImage = QImage( size, desiredFormat );
    }
    auto bytesPerLine = qImage.bytesPerLine();
    CARTA_ASSERT( bytesPerLine == size.width() * 4 );
    Q_UNUSED( bytesPerLine );

    // start with a pointer to the beginning of last row (we are constructing image
    // bottom-up)
    QRgb * outPtr = reinterpret_cast < QRgb * > (
        qImage.bits() + size.width() * ( size.height() - 1 ) * 4 );

    if( 0) {
        CARTA_ASSERT( qImage.bits() + size.width() * (size.height() - 1) * 4 == qImage.scanLine( size.height() - 1));
        // sanity check
        for( int y = 0 ; y < size.height() ; y ++ ) {
            CARTA_ASSERT( qImage.bits() + size.width() * 4 * y == qImage.scanLine(y) );
        }
    }

    // make a double view
    NdArray::TypedView < Scalar > typedView( rawView, false );

    /// @todo for more efficiency, instead of forEach() we should switch to one of the
    /// higher performance APIs and maybe even sprinkle it with some openmp/cilk magic :)
    int64_t counter = 0;


    auto lambda = [&] ( const Scalar & ival )
    {
        if ( Q_LIKELY( ! std::isnan( ival ) ) ) {
            pipe.convertq( ival, * outPtr );
        }
        else {
            * outPtr = nanColor;
        }
        outPtr++;
        counter++;

        // build the image bottom-up
        if ( counter % size.width() == 0 ) {
            outPtr -= size.width() * 2;
        }
    };
    typedView.forEach( lambda );

    CARTA_ASSERT( counter == size.width() * size.height());

} // rawView2QImage

namespace Carta
{
namespace Core
{
namespace ImageRenderService
{
void
Service::setInputView( NdArray::RawViewInterface::SharedPtr view, QString cacheId )
{
    m_inputView = view;

    m_inputViewCacheId = cacheId;
    m_frameImage = QImage(); // indicate a need to recompute
}

void
Service::setOutputSize( QSize size )
{
    m_outputSize = size;
}

QSize
Service::outputSize() const
{
    return m_outputSize;
}

void Service::setNanColor( QColor color ){
    if ( m_nanColor != color ){
        m_nanColor = color;
    }
}

QColor Service::getNanColor() const {
    QRgb nanColor = m_nanColor.rgb();
    if ( m_pixelPipelineRaw && m_defaultNan){
        double clipMin, clipMax;
        m_pixelPipelineRaw-> getClips( clipMin, clipMax );
        m_pixelPipelineRaw->convertq( clipMin, nanColor );
   }
    return QColor( nanColor );
}

void Service::setDefaultNan( bool useNanDefault){
    m_defaultNan = useNanDefault;
}

void
Service::setPan( QPointF pt )
{
    m_pan = pt;
}

QPointF
Service::pan()
{
    return m_pan;
}

void
Service::setZoom( double zoom )
{
    double newZoom = Carta::Lib::clamp( zoom, 1e-9, 1e+9 );
    if ( newZoom != m_zoom ) {
        m_zoom = newZoom;
    }
}

double
Service::zoom()
{
    return m_zoom;
}

void
Service::setPixelPipeline( IClippedPixelPipeline::SharedPtr pixelPipeline,
                           QString cacheId = QString() )
{
    m_pixelPipelineRaw = pixelPipeline;
    m_pixelPipelineCacheId = cacheId;

    // invalidate frame cache
    m_frameImage = QImage();

    // invalidate pixel pipeline cache
    m_cachedPP = nullptr;
    m_cachedPPinterp = nullptr;
}

void
Service::setPixelPipelineCacheSettings( const PixelPipelineCacheSettings & params )
{
    m_pixelPipelineCacheSettings = params;

    // invalidate frame cache
    m_frameImage = QImage();

    // invalidate pixel pipeline cache
    m_cachedPP = nullptr;
    m_cachedPPinterp = nullptr;
}

const Service::PixelPipelineCacheSettings &
Service::pixelPipelineCacheSettings() const
{
    return m_pixelPipelineCacheSettings;
}

JobId
Service::render( JobId jobId )
{
    if ( jobId < 0 ) {
        m_lastSubmittedJobId++;
    }
    else {
        m_lastSubmittedJobId = jobId;
    }
    if ( ! m_renderTimer.isActive() ) {
        m_renderTimer.start();
    }
    return m_lastSubmittedJobId;
}

Service::Service( QObject * parent ) : Carta::Lib::IImageRenderService( parent ),
        m_defaultNan( true ),
        m_nanColor( 255, 0, 0 )
{
    // hook up the internal schedule helper signal to the scheduleJob slot, using
    // queued connection
//    auto conn = connect( this, & Service::internalRenderSignal,
//                         this, & Service::internalRenderSlot,
//                         Qt::QueuedConnection );

    m_renderTimer.setSingleShot( true );
    m_renderTimer.setInterval( 1 );
    connect( & m_renderTimer, & QTimer::timeout, this, & Me::internalRenderSlot );

    m_frameCache.setMaxCost( 1 * 1024 * 1024 * 1024 ); // 1 gig
}

Service::~Service()
{ }

QPointF
Service::img2screen( const QPointF & p )
{
    return image2screen( p, m_pan, m_zoom, m_outputSize );
}

QPointF
Service::image2screen( const QPointF& p, const QPointF& pan,
        double zoom, const QSize& outputSize ) const {
    double icx = pan.x();
    double scx = outputSize.width() / 2.0;
    double icy = pan.y();
    double scy = outputSize.height() / 2.0;

    /// \todo cache xmap/ymap, update with zoom/pan/resize
    Carta::Lib::LinearMap1D xmap( scx, scx + zoom, icx, icx + 1 );
    Carta::Lib::LinearMap1D ymap( scy, scy + zoom, icy, icy - 1 );
    QPointF res;
    res.rx() = xmap.inv( p.x() );
    res.ry() = ymap.inv( p.y() );
    return res;
}

QPointF
Service::screen2img( const QPointF & p )
{
    return screen2image( p, m_pan, m_zoom, m_outputSize );
}

QPointF
Service::screen2image( const QPointF & p, const QPointF& pan, double zoom,
        const QSize& outputSize ) const {
    double icx = pan.x();
    double scx = outputSize.width() / 2.0;
    double icy = pan.y();
    double scy = outputSize.height() / 2.0;

    /// \todo cache xmap/ymap, update with zoom/pan/resize

    Carta::Lib::LinearMap1D xmap( scx, scx + zoom, icx, icx + 1 );
    Carta::Lib::LinearMap1D ymap( scy, scy + zoom, icy, icy - 1 );
    QPointF res;
    res.rx() = xmap.apply( p.x() );
    res.ry() = ymap.apply( p.y() );
    return res;
}

void
Service::internalRenderSlot()
{
    //static int renderCount = 0;
    //qDebug() << "Image render" << renderCount++ << "xyz";

    // raw double to base64 converter
    auto d2hex = [] (double x) -> QString {
        return QByteArray( (char *) ( & x ), sizeof( x ) ).toBase64();
    };

    double clipMin, clipMax;
    m_pixelPipelineRaw-> getClips( clipMin, clipMax );

    QRgb nanColor = m_nanColor.rgb();
    if ( m_defaultNan ){
        m_pixelPipelineRaw->convertq( clipMin, nanColor );
    }

    // cache id will be concatenation of:
    // view id
    // pipeline id
    // output size
    // pan
    // zoom
    // nan
    // pixel pipeline cache settings
    // Floats are binary-encoded (base64)
    QString cacheId = QString( "%1/%2/%3x%4/%5,%6/%7/%8" )
                          .arg( m_inputViewCacheId )
                          .arg( m_pixelPipelineCacheId )
                          .arg( m_outputSize.width() )
                          .arg( m_outputSize.height() )
                          .arg( d2hex( m_pan.x() ) )
                          .arg( d2hex( m_pan.y() ) )
                          .arg( d2hex( m_zoom ) )
                          .arg( QString::number(nanColor) );


    if ( m_pixelPipelineCacheSettings.enabled ) {
        cacheId += QString( "/1/%1/%2" )
                       .arg( int (m_pixelPipelineCacheSettings.interpolated) )
                       .arg( m_pixelPipelineCacheSettings.size );
    }
    else {
        cacheId += "/0";
    }

//    qDebug() << "internalRenderSlot... cache size: "
//             << m_frameCache.totalCost() * 100.0 / m_frameCache.maxCost() << "% "
//             << m_frameCache.size() << "entries";
//    qDebug() << "id:" << cacheId;
    struct Scope {
        ~Scope() { /*qDebug() << "internalRenderSlot done";*/ } }
    debugScopeGuard;

    auto cachedImage = m_frameCache.object( cacheId );
    if ( cachedImage ) {
        //qDebug() << "frame cache hit";
        emit done( * cachedImage, m_lastSubmittedJobId );
        return;
    }
    //qDebug() << "frame cache miss";

    if ( ! m_inputView ) {
        qCritical() << "input view not set";
        qDebug() << "xyz internal renderslot" << m_inputView.get() << this;
        return;
    }

    if ( ! m_pixelPipelineRaw ) {
        qCritical() << "pixel pipeline not set";
        return;
    }



    // render the frame if needed
    if ( m_frameImage.isNull() ) {

        if ( pixelPipelineCacheSettings().enabled ) {
            if ( pixelPipelineCacheSettings().interpolated ) {
                if ( ! m_cachedPPinterp ) {
                    m_cachedPPinterp.reset( new Lib::PixelPipeline::CachedPipeline < true > () );
                    m_cachedPPinterp-> cache( * m_pixelPipelineRaw,
                            pixelPipelineCacheSettings().size, clipMin, clipMax );
                }
                ::iView2qImage( m_inputView.get(), * m_cachedPPinterp, m_frameImage, nanColor );
            }
            else {
                if ( ! m_cachedPP ) {
                    m_cachedPP.reset( new Lib::PixelPipeline::CachedPipeline < false > () );
                    m_cachedPP-> cache( * m_pixelPipelineRaw,
                            pixelPipelineCacheSettings().size, clipMin, clipMax );
                }
                ::iView2qImage( m_inputView.get(), * m_cachedPP, m_frameImage, nanColor );
            }
        }
        else {
            ::iView2qImage( m_inputView.get(), * m_pixelPipelineRaw, m_frameImage, nanColor );
        }
    }

    // prepare output
    QImage img( m_outputSize, OptimalQImageFormat );
    if ( m_outputSize.width() > 0 && m_outputSize.height() > 0 ){

        //    img.fill( QColor( "blue" ) );
        img.fill( QColor( 50, 50, 50 ) );
        QPainter p( & img );

        // draw the frame image to satisfy zoom/pan
        //    QPointF p1 = img2screen( QPointF( -0.5, -0.5 ) );
        //    QPointF p2 = img2screen( QPointF( m_frameImage.width()-0.5, m_frameImage.height()-0.5));

        int imageHeight = m_frameImage.height();
        QPointF p1 = img2screen( QPointF( - 0.5, imageHeight - 0.5 ) );
        QPointF p2 = img2screen( QPointF( m_frameImage.width() - 0.5, - 0.5 ) );

        QRectF rectf( p1, p2 );
        p.setRenderHint( QPainter::SmoothPixmapTransform, false );

        //    rectf = rectf.normalized();
        p.drawImage( rectf, m_frameImage );

        //    qDebug() << "m_frameImage" << m_frameImage.size();
        //    qDebug() << "m_frameImage" << zoom() << rectf.width() / m_frameImage.width()
        //             << rectf.height() / m_frameImage.height();

        // debugging rectangle
        if ( 0 ) {
            p.setPen( QPen( QColor( "yellow" ), 3 ) );
            p.setBrush( Qt::NoBrush );
            p.drawRect( rectf );
        }

        // more debugging - draw pixel grid
        // \todo need to add clipping if we want to expose this as a functionality
        if ( true && zoom() > 5 ) {
            p.setRenderHint( QPainter::Antialiasing, true );
            double alpha = Carta::Lib::linMap( zoom(), 5, 32, 0.01, 0.2 );
            //qDebug() << "alpha="<<alpha;
            alpha = Carta::Lib::clamp( alpha, 0.0, 1.0 );
            p.setPen( QPen( QColor( 255, 255, 255, 255 ), alpha ) );
            QPointF tl = screen2img( QPointF( 0, 0 ) );
            QPointF br = screen2img( QPointF( outputSize().width(), outputSize().height() ) );
            int x1 = std::floor( tl.x() );
            int x2 = std::ceil( br.x() );
            //qDebug() << "x1="<<x1<<" x2="<<x2;
            for ( double x = x1 ; x <= x2 ; ++x ) {
                QPointF pt = img2screen( QPointF( x - 0.5, 0 ) );
                p.drawLine( QPointF( pt.x(), 0 ), QPointF( pt.x(), outputSize().height() ) );
            }
            int y1 = std::ceil( tl.y() );
            int y2 = std::floor( br.y() );
            std::swap( y1, y2 );
            for ( double y = y1 ; y <= y2 ; ++y ) {
                QPointF pt = img2screen( QPointF( 0, y - 0.5 ) );
                p.drawLine( QPointF( 0, pt.y() ), QPointF( outputSize().width(), pt.y() ) );
            }
        }
        // debuggin: put a yellow stamp on the image, so that next time it's recalled
        // it'll have 'cached' stamped on it
        if ( CARTA_RUNTIME_CHECKS ) {
            p.setPen( QColor( "yellow" ) );
            p.drawText( img.rect(), Qt::AlignRight | Qt::AlignBottom, "Cached" );
        }
    }

    // report result
    emit done( img, m_lastSubmittedJobId );



    // insert this image into frame cache
    m_frameCache.insert( cacheId, new QImage( img ), img.byteCount() );

} // internalRenderSlot

}
}
}
