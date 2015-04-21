/**
 *
 **/

#include "ImageRenderService.h"
#include "LinearMap.h"
#include <QColor>
#include <QPainter>

/// algorithm for converting an instance of image interface to qimage
/// using the pixel pipeline
///
/// \tparam Pipeline
/// \param m_rawView
/// \param pipe
/// \param m_qImage
template < class Pipeline >
static void
rawView2QImage2( NdArray::RawViewInterface * rawView, Pipeline & pipe, QImage & qImage )
{
    qDebug() << "rv2qi2" << rawView-> dims();
    typedef double Scalar;
    QSize size( rawView->dims()[0], rawView->dims()[1] );

    if ( qImage.format() != QImage::Format_ARGB32 ||
         qImage.size() != size ) {
        qImage = QImage( size, QImage::Format_ARGB32 );
    }
    auto bytesPerLine = qImage.bytesPerLine();
    CARTA_ASSERT( bytesPerLine == size.width() * 4 );

    // start with a pointer to the beginning of last row (we are constructing image
    // bottom-up)
    QRgb * outPtr = reinterpret_cast < QRgb * > (
        qImage.bits() + size.width() * ( size.height() - 1 ) * 4 );

    // make a double view
    NdArray::TypedView < Scalar > typedView( rawView, false );

    /// @todo for more efficiency we should switch to the higher performance view apis
    /// and apply some basic openmp/cilk
    int64_t counter = 0;
    QRgb nanColor = qRgb( 255, 0, 0 );
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
Service::getOutputSize()
{
    return m_outputSize;
}

void
Service::setPan( QPointF pt )
{
    if ( pt != m_pan ) {
        m_pan = pt;
    }
}

QPointF Service::pan()
{
    return m_pan;
}

void
Service::setZoom( double zoom )
{
    double newZoom = clamp( zoom, 0.1, 64.0 );
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

const PixelPipelineCacheSettings &
Service::pixelPipelineCacheSettings() const
{
    return m_pixelPipelineCacheSettings;
}

void
Service::render( JobId jobId )
{
    emit internalRenderSignal( jobId );
}

Service::Service( QObject * parent ) : QObject( parent )
{
    // hook up the internal schedule helper signal to the scheduleJob slot, using
    // queued connection
    auto conn = connect( this, & Service::internalRenderSignal,
                         this, & Service::internalRenderSlot,
                         Qt::QueuedConnection );

    m_frameCache.setMaxCost( 1 * 1024 * 1024 * 1024); // 1 gig
}

Service::~Service()
{ }

QPointF
Service::img2screen( const QPointF & p )
{
    double icx = m_pan.x();
    double scx = m_outputSize.width() / 2.0;
    double icy = m_pan.y();
    double scy = m_outputSize.height() / 2.0;
    LinearMap1D xmap( scx, scx + m_zoom, icx, icx + 1 );
    LinearMap1D ymap( scy, scy + m_zoom, icy, icy + 1 );
    QPointF res;
    res.rx() = xmap.inv( p.x() );
    res.ry() = ymap.inv( p.y() );
    return res;
}

QPointF
Service::screen2img( const QPointF & p )
{
    double icx = m_pan.x();
    double scx = m_outputSize.width() / 2.0;
    double icy = m_pan.y();
    double scy = m_outputSize.height() / 2.0;
    LinearMap1D xmap( scx, scx + m_zoom, icx, icx + 1 );
    LinearMap1D ymap( scy, scy + m_zoom, icy, icy + 1 );
    QPointF res;
    res.rx() = xmap.apply( p.x() );
    res.ry() = ymap.apply( p.y() );
    return res;
}

bool
Service::saveFullImage( const QString& filename, double scale )
{
    if ( m_frameImage.isNull() ) {
        _renderFrame();
    }
    bool result = m_frameImage.scaled(m_frameImage.size() * scale).save( filename );
    return result;
}

void
Service::internalRenderSlot( JobId jobId )
{
    auto d2hex = [] (double x) -> QString {
        return QByteArray( (char *)(& x), sizeof(x)).toBase64();
    };

    // cache id will be concatenation of:
    // view id
    // pipeline id
    // output size
    // pan
    // zoom
    // pixel pipeline cache settings
    // Floats are binary-encoded (base64)

    QString cacheId = QString( "%1/%2/%3x%4/%5,%6/%7" )
                          .arg( m_inputViewCacheId )
                          .arg( m_pixelPipelineCacheId )
                          .arg( m_outputSize.width() )
                          .arg( m_outputSize.height() )
                          .arg( d2hex(m_pan.x()))
                          .arg( d2hex(m_pan.y()))
                          .arg( d2hex(m_zoom));

    if ( m_pixelPipelineCacheSettings.enabled ) {
        cacheId += QString( "/1/%1/%2" )
                   .arg( int(m_pixelPipelineCacheSettings.interpolated))
                   .arg( m_pixelPipelineCacheSettings.size);
    }
    else {
        cacheId += "/0";
    }

    qDebug() << "internalRenderSlot... cache size: "
             << m_frameCache.totalCost() * 100.0 / m_frameCache.maxCost() << "% "
             << m_frameCache.size() << "entries";
    qDebug() << "id:" << cacheId;
    struct Scope {
        ~Scope() { qDebug() << "internalRenderSlot done"; } }
    debugScopeGuard;


    auto cachedImage = m_frameCache.object( cacheId );
    if ( cachedImage ) {
        qDebug() << "frame cache hit";
        emit done( * cachedImage, jobId );
        return;
    }
    qDebug() << "frame cache miss";

    if ( ! m_inputView ) {
        qCritical() << "input view not set";
        return;
    }

    if ( ! m_pixelPipelineRaw ) {
        qCritical() << "pixel pipeline not set";
        return;
    }

    // render the frame if needed
    if ( m_frameImage.isNull() ) {
        _renderFrame();
    }

    // prepare output
    QImage img( m_outputSize, QImage::Format_ARGB32 );
    img.fill( QColor( "#E0E0E0" ) );
    QPainter p( & img );

    // draw the frame image to satisfy zoom/pan
    QPointF p1 = img2screen( QPointF( 0, 0 ) );
    QPointF p2 = img2screen( QPointF( m_frameImage.width(), m_frameImage.height() ) );
    QRectF rectf( p1, p2 );
    p.drawImage( rectf, m_frameImage );

    // debug code: redraw counter
//    if ( CARTA_RUNTIME_CHECKS ) {
//        static int counter = 0;
//        counter++;
//        p.setPen( QColor( "yellow" ) );
//        p.drawText( img.rect(), Qt::AlignCenter, QString::number( counter ) );
//    }

    // report result
    emit done( QImage(img), jobId );

//    stamp the image about to be inserted with
    if ( CARTA_RUNTIME_CHECKS ) {
        static int counter = 0;
        counter++;
        //p.setPen( QColor( "yellow" ) );
        //p.drawText( img.rect(), Qt::AlignCenter, "Cached" );
    }

    // insert this image into frame cache
    qDebug() << "byteCount=" << img.byteCount();
    m_frameCache.insert( cacheId, new QImage( img ), img.byteCount());
} // internalRenderSlot

void
Service::_renderFrame()
{
    double clipMin, clipMax;
    m_pixelPipelineRaw-> getClips( clipMin, clipMax );
    if ( pixelPipelineCacheSettings().enabled ) {
        if ( pixelPipelineCacheSettings().interpolated ) {
            if ( ! m_cachedPPinterp ) {
                m_cachedPPinterp.reset( new Lib::PixelPipeline::CachedPipeline < true > () );
                m_cachedPPinterp-> cache( * m_pixelPipelineRaw,
                                          pixelPipelineCacheSettings().size, clipMin, clipMax );
            }
            ::rawView2QImage2( m_inputView.get(), * m_cachedPPinterp, m_frameImage );
        }
        else {
            if ( ! m_cachedPP ) {
                m_cachedPP.reset( new Lib::PixelPipeline::CachedPipeline < false > () );
                m_cachedPP-> cache( * m_pixelPipelineRaw,
                                    pixelPipelineCacheSettings().size, clipMin, clipMax );
            }
            ::rawView2QImage2( m_inputView.get(), * m_cachedPP, m_frameImage );
        }
    }
    else {
        ::rawView2QImage2( m_inputView.get(), * m_pixelPipelineRaw, m_frameImage );
    }
} // _renderFrame

}
}
}
