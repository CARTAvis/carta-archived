/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include "CartaLib/IImage.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"
#include "quantileAlgorithms.h"
#include <QImage>
#include <QCache>

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
class RawView2QImageConverter
{
    CLASS_BOILERPLATE( RawView2QImageConverter);

public:

//    typedef RawView2QImageConverter & Me;
    typedef double Scalar;

    RawView2QImageConverter();

    ~RawView2QImageConverter();

    /// \warning the pointer is kept for invocation of go(). Make sure it's valid
    /// during a call to go()!
    Me &
    setView( NdArray::RawViewInterface * rawView );

//    Me &
//    setFrame( int frame );

    Me &
    setAutoClip( double val );

    Me &
    setColormap( Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr cmap );

    const QImage &
    go( int frame, bool recomputeClip = true );

protected:

    struct CachedImage;
    QImage m_qImage;
    Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr m_cmap;
    NdArray::RawViewInterface * m_rawView = nullptr;
    double m_autoClip = 0.95;

    QCache < int, CachedImage > m_cache;
    double m_clip1 = 1.0 / 0.0, m_clip2 = 0.0;
};

/// algorithm for converting an instance of image interface to qimage
/// using the pixel pipeline
///
/// \tparam Pipeline
/// \param m_rawView
/// \param pipe
/// \param m_qImage
template < class Pipeline >
void
rawView2QImage( NdArray::RawViewInterface * rawView, Pipeline & pipe, QImage & qImage )
{
    qDebug() << "rv2qi" << rawView-> dims();
    typedef double Scalar;
    QSize size( rawView->dims()[0], rawView->dims()[1] );

    if ( qImage.format() != QImage::Format_ARGB32 ||
         qImage.size() != size ) {
        qImage = QImage( size, QImage::Format_ARGB32 );
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


/// controlling raw data to image conversion:
/// - invert on/off
/// - reverse on/off
/// - change colormap IColormapScalar
/// - log scaling/gamma-correction/casa-cycles enum
/// - manual clip setting
/// - percentage based clip computation
/// - pipeline caching (on/off/size/interpolation)
/// - image caching (on/off/size)
/// - user defined stages
/// - stages implementable through plugins

namespace Carta
{
namespace Core
{
class RawView2QImageConverter3
{
    CLASS_BOILERPLATE( RawView2QImageConverter3 );

public:

    RawView2QImageConverter3();

    /// what size cache should we use (in bytes) for image caching
    Me &
    setImageCacheSize( int64_t size );

    /// how many entries to use for pipeline caching
    Me &
    setPixelPipelineCacheSize( int64_t size ) {
        if( size < 2) size = 2;
        m_cmapCacheSize = size;
        return * this;
    }

    /// whether to use linear interpolation for pixel pipeline cache
    Me &
    setPixelPipelineInterpolation( bool flag ) {
        m_cmapCachingInterpolated = flag;
        return * this;
    }

    /// whether to use pipeline cache at all
    Me &
    setPixelPipelineCacheEnabled( bool flag ) {
        m_cmapCachingEnabled = flag;
        return * this;
    }

    /// Sets the view on which we'll operate
    /// @param rawView pointer to the raw view, caller retains ownership but read
    /// the warning below
    /// @param id string unique for the view, as it will be used by caching
    /// it could be something like "/home/john/file.fits//0:20:10,::-1,1:20:2"
    /// \warning the pointer is kept for invocations of convert() and computeClips().
    /// Make sure it's valid
    /// during a call to go()!
    Me &
    setView( NdArray::RawViewInterface * rawView, QString id = "" );

    /// compute clips using the current view (set by setView())
    /// @param clip for example 0.95 means 95%
    Me &
    computeClips( double clip );

    /// manual setting of clips
    Me &
    setClips( double min, double max );

    Me &
    setInvert( bool flag )
    ;

    Me &
    setReverse( bool flag )
    ;

    Me &
    setColormap( Lib::PixelPipeline::IColormapNamed::SharedPtr colormap )
    ;

    QString getCmapPreview( int n = 10);

    /// do the actual conversion with the previously set parameters
    void convert(QImage & img);

private:
    /// pointer to the view
//    NdArray::RawViewInterface * m_rawView = nullptr;
    NdArray::TypedView<double>::UniquePtr m_typedView;
    /// cache for images
    QCache < QString, QImage > m_imageCache;
    /// unoptimized pipeline
    Lib::PixelPipeline::CustomizablePixelPipeline::UniquePtr m_customPipeline;
    /// optimized pipeline
//    Lib::PixelPipeline::CachedPipeline::UniquePtr m_cachedPipeline;

    /// do pixel caching or not
    bool m_cmapCachingEnabled = true;
    /// do pixel caching using interpolation or not
    bool m_cmapCachingInterpolated = false;
    /// requested cache size for pixel pipepline caching
    int m_cmapCacheSize = 1000;

    double m_clipMin = 0.0, m_clipMax = 1.0;

};
}
}
