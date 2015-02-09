/// \todo This is getting way too object oriented. We should probably refactor this into

/// pure template implementation to regain some speed via vectorization, at least for
/// the cached pixel transfer function case.

#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include "CartaLib/TPixelPipeline/IScalar2Scalar.h"

#include "CartaLib/CartaLib.h"
#include "GrayColormap.h"
#include "RawView2QImageConverter.h"

#include <functional>
#include <array>
#include <QColor>

struct RawView2QImageConverter::CachedImage {
    QImage img;
    double clip1, clip2, perc;
    bool   specificToFrame;

    CachedImage( QImage pImg, double c1, double c2, bool spec, double p )
    {
        img = pImg;
        clip1 = c1;
        clip2 = c2;
        specificToFrame = spec;
        perc = p;
    }
};

RawView2QImageConverter::RawView2QImageConverter()
{
    // set cache size (in MB)
//    m_cache.reset( new QCache < int, CachedImage > () );
    m_cache.setMaxCost( 6 * 1024 );
    m_cmap = std::make_shared < Carta::Core::GrayColormap > ();
}

RawView2QImageConverter::~RawView2QImageConverter()
{ }

RawView2QImageConverter::Me &
RawView2QImageConverter::setView( NdArray::RawViewInterface * rawView )
{
    if ( ! rawView ) {
        throw std::runtime_error( "iimage is null" );
    }
    if ( rawView->dims().size() < 2 ) {
        throw std::runtime_error( "need at least 2 dimensions" );
    }
    m_rawView = rawView;
    return * this;
}

//RawView2QImageConverter::Me &
//RawView2QImageConverter::setFrame( int frame )
//{
//    if ( frame > 0 ) {
//        int nFrames = 0;
//        if ( m_rawView->dims().size() > 2 ) {
//            nFrames = m_rawView->dims()[2];
//        }
//        if ( frame >= nFrames ) {
//            throw std::runtime_error( "not enough frames in this image" );
//        }
//    }
//    return * this;
//}

RawView2QImageConverter::Me &
RawView2QImageConverter::setAutoClip( double val )
{
    m_autoClip = val;
    return * this;
}

RawView2QImageConverter::Me &
RawView2QImageConverter::setColormap( Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr cmap )
{
    m_cmap = cmap;
    m_cache.clear();
    return * this;
}

const QImage &
RawView2QImageConverter::go( int frame, bool recomputeClip )
{
    // let's see if we have the result in cache
    CachedImage * img = m_cache.object( frame );
    if ( img ) {
        bool match = false;

        // we have something in cache, let's see if it matches what we want
        if ( recomputeClip ) {
            if ( img->specificToFrame && img->perc == m_autoClip ) {
                match = true;
            }
        }
        else if ( img->clip1 == m_clip1 && img->clip2 == m_clip2 ) {
            match = true;
        }

        // if we found a match, return it and we're done
        if ( match ) {
            qDebug() << "Cache hit #" << frame;
            return img->img;
        }
    }
    qDebug() << "Cache miss #" << frame;

    NdArray::TypedView < Scalar > view( m_rawView, false );

    if ( ! std::isfinite( m_clip1 ) || recomputeClip ) {
        std::tie( m_clip1, m_clip2 ) = computeClips < Scalar > ( view, m_autoClip );
    }
    int width = m_rawView->dims()[0];
    int height = m_rawView->dims()[1];

    // make sure there is at least 1 finite value, if not, return a black image
    if ( ! std::isfinite( m_clip1 ) ) {
        // there were no values.... return a black image
        m_qImage = QImage( width, height, QImage::Format_ARGB32 );
        m_qImage.fill( 0 );
        return m_qImage;
    }

    // construct image using clips (clipd, clipinv)
    m_qImage = QImage( width, height, QImage::Format_ARGB32 );
    auto bytesPerLine = m_qImage.bytesPerLine();
    if ( bytesPerLine != width * 4 ) {
        throw std::runtime_error( "qimage does not have consecutive memory..." );
    }
    QRgb * outPtr = reinterpret_cast < QRgb * > ( m_qImage.bits() + width * ( height - 1 ) * 4 );

    // precalculate linear transformation coeff.
    Scalar clipdinv = 1.0 / ( m_clip2 - m_clip1 );

    // apply the clips
    CARTA_ASSERT( m_cmap != nullptr );
    int64_t counter = 0;
    Carta::Lib::PixelPipeline::NormRgb normRgb;
    auto lambda = [&] ( const Scalar & ival )
    {
        if ( std::isfinite( ival ) ) {
            Scalar val = ( ival - m_clip1 ) * clipdinv;
            val = clamp < Scalar > ( val, 0.0, 1.0 );

//            * outPtr = m_cmap->convert( val );
            m_cmap->convert( val, normRgb );
            Carta::Lib::PixelPipeline::normRgb2QRgb( normRgb, * outPtr );
        }
        else {
            * outPtr = qRgb( 255, 0, 0 );
        }
        outPtr++;
        counter++;

        // build the image bottom-up
        if ( counter % width == 0 ) {
            outPtr -= width * 2;
        }
    };
    view.forEach( lambda );

    // stick this into the cache so we don't need to recompute it
    m_cache.insert( frame,
                    new CachedImage( m_qImage, m_clip1, m_clip2, recomputeClip, m_autoClip ),
                    width * height * 4 / 1024 / 1024 );

    qDebug() << "cache total=" << m_cache.totalCost() << "MB";

    return m_qImage;
} // go

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
