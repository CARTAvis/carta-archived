/// \todo This is getting way too object oriented. We should probably refactor this into

/// pure template implementation to regain some speed via vectorization, at least for
/// the cached pixel transfer function case.

#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include "CartaLib/CartaLib.h"
#include "GrayColormap.h"
#include "RawView2QImageConverter.h"

#include <functional>
#include <array>
#include <QColor>

template < typename Scalar >
static
typename std::tuple < Scalar, Scalar >
computeClips(
    NdArray::TypedView < Scalar > & view,
    double perc
    )
{
    qDebug() << "perc=" << perc;
    perc = (1 - perc) / 2;
    std::vector<Scalar> res = Carta::Core::Algorithms::quantiles2pixels(
                                  view, { perc, 1 - perc});
    return std::make_tuple( res[0], res[1]);
}


/// algorithm for converting an instance of image interface to qimage
/// using the pixel pipeline
///
/// \tparam Pipeline
/// \param m_rawView
/// \param pipe
/// \param m_qImage
template < class Pipeline >
static void
rawView2QImage( NdArray::RawViewInterface * rawView, Pipeline & pipe, QImage & qImage )
{
    qDebug() << "rv2qi" << rawView-> dims();
    typedef double Scalar;
    QSize size( rawView->dims()[0], rawView->dims()[1] );

    if ( qImage.format() != QImage::Format_ARGB32_Premultiplied ||
         qImage.size() != size ) {
        qImage = QImage( size, QImage::Format_ARGB32_Premultiplied );
    }

    // construct image
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
    QRgb nanColor = qRgb( 255, 0, 0);
    auto lambda = [&] ( const Scalar & ival )
    {
        if( ! std::isnan( ival)) {
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
RawView2QImageConverter3::RawView2QImageConverter3()
{
    m_customPipeline.reset( new Lib::PixelPipeline::CustomizablePixelPipeline );
}

RawView2QImageConverter3 & RawView2QImageConverter3::setInvert(bool flag)
{
    CARTA_ASSERT( m_customPipeline );
    m_customPipeline->setInvert( flag );
    return * this;
}

RawView2QImageConverter3 & RawView2QImageConverter3::setReverse(bool flag)
{
    CARTA_ASSERT( m_customPipeline );
    m_customPipeline->setReverse( flag );
    return * this;
}

QString
RawView2QImageConverter3::getCmapPreview( int n )
{
    QStringList list;
    for ( int i = 0 ; i <= n ; i++ ) {
        double x = ( m_clipMax - m_clipMin ) / n * i + m_clipMin;
        QRgb col;
        m_customPipeline-> convertq( x, col );
        list << QColor( col ).name().mid( 1 );
    }
    return list.join( "," );
}

/// @todo some optimization would be useful here, i.e. no need to recompute the cached
/// pipeline every single time... only when related parameters are changed, but this would
/// require some interesting refactoring to not loose the template speedup, without
/// affecting memory consumption
void
RawView2QImageConverter3::convert( QImage & img )
{
    CARTA_ASSERT( m_typedView );
    CARTA_ASSERT( m_customPipeline );

    if ( m_cmapCachingEnabled ) {
        if ( m_cmapCachingInterpolated ) {
            Lib::PixelPipeline::CachedPipeline < true > cachedPixPipe;
            cachedPixPipe.cache( * m_customPipeline, m_cmapCacheSize, m_clipMin, m_clipMax );
            ::rawView2QImage( m_typedView-> rawView(), cachedPixPipe, img );
        }
        else {
            Lib::PixelPipeline::CachedPipeline < false > cachedPixPipe;
            cachedPixPipe.cache( * m_customPipeline, m_cmapCacheSize, m_clipMin, m_clipMax );
            ::rawView2QImage( m_typedView-> rawView(), cachedPixPipe, img );
        }
    }
    else {
        ::rawView2QImage( m_typedView-> rawView(), * m_customPipeline, img );
    }
} // convert

RawView2QImageConverter3 &
RawView2QImageConverter3::setColormap( Lib::PixelPipeline::IColormapNamed::SharedPtr colormap )
{
    CARTA_ASSERT( m_customPipeline );
    CARTA_ASSERT( colormap );
    m_customPipeline->setColormap( colormap );
    return * this;
}

RawView2QImageConverter3 &
RawView2QImageConverter3::setView( NdArray::RawViewInterface * rawView, QString id )
{
    Q_UNUSED( id );
    m_typedView.reset( new NdArray::TypedView < double > ( rawView, false ) );
    return * this;
}

RawView2QImageConverter3 &
RawView2QImageConverter3::computeClips( double clip )
{
    CARTA_ASSERT( m_typedView );
    double min, max;
    std::tie( min, max ) = ::computeClips < double > ( * m_typedView, clip );
    setClips( min, max );
    return * this;
}

RawView2QImageConverter3 &
RawView2QImageConverter3::setClips( double min, double max )
{
    CARTA_ASSERT( m_customPipeline );
    m_clipMin = min;
    m_clipMax = max;
    m_customPipeline-> setMinMax( min, max );
    return * this;
}
}
}
