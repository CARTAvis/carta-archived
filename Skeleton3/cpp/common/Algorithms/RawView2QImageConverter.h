/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include "CartaLib/IColormapScalar.h"
#include "CartaLib/IImage.h"
#include <QImage>
#include <QCache>

/// compute clip values from the values in a view
template < typename Scalar >
static
typename std::tuple < Scalar, Scalar >
computeClips(
    NdArray::TypedView < Scalar > & view,
    double perc
    )
{
    // read in all values from the view into an array
    // we need our own copy because we'll do quickselect on it...
    std::vector < Scalar > allValues;
    view.forEach(
        [& allValues] ( const Scalar & val ) {
            if ( std::isfinite( val ) ) {
                allValues.push_back( val );
            }
        }
        );

    // indicate bad clip if no finite numbers were found
    if ( allValues.size() == 0 ) {
        return std::make_tuple(
                   std::numeric_limits < Scalar >::quiet_NaN(),
                   std::numeric_limits < Scalar >::quiet_NaN() );
    }
    Scalar clip1, clip2;
    Scalar hist = ( 1.0 - perc ) / 2.0;
    int x1 = allValues.size() * hist;
    std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end() );
    clip1 = allValues[x1];
    x1 = allValues.size() - 1 - x1;
    std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end() );
    clip2 = allValues[x1];

    if ( 0 ) {
        // debug
        int64_t count = 0;
        for ( auto & x : allValues ) {
            if ( x >= clip1 && x <= clip2 ) {
                count++;
            }
        }
        qDebug() << "autoClip(" << perc << ")=" << count * 100.0 / allValues.size();
    }

    return std::make_tuple( clip1, clip2 );
} // computeClips

/// algorithm for converting an instance of image interface to qimage
class RawView2QImageConverter
{
public:

    typedef RawView2QImageConverter & Me;
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
    setColormap( Carta::Lib::IColormapScalar::SharedPtr cmap );

    const QImage &
    go( int frame, bool recomputeClip = true );

protected:

    struct CachedImage;
    QImage m_qImage;
    Carta::Lib::IColormapScalar::SharedPtr m_cmap;
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
    typedef double Scalar;
    QSize size( rawView->dims()[0], rawView->dims()[1] );

    if ( qImage.format() != QImage::Format_ARGB32 ||
         qImage.size() != size ) {
        qImage = QImage( size, QImage::Format_ARGB32 );
    }

    // construct image using clips (clipd, clipinv)
    auto bytesPerLine = qImage.bytesPerLine();
    CARTA_ASSERT( bytesPerLine == size.width() * 4 );

    // start with a pointer to the beginning of last row (we are constructing image
    // bottom-up)
    QRgb * outPtr = reinterpret_cast < QRgb * > (
        qImage.bits() + size.width() * ( size.height() - 1 ) * 4 );

    // make a double view
    NdArray::TypedView < Scalar > view( rawView, false );

    /// @todo for more efficiency we should switch to the higher performance view apis
    /// and apply some basic openmp/cilk
    int64_t counter = 0;
    auto lambda = [&] ( const Scalar & ival )
    {
        pipe.convertq( ival, * outPtr );
        outPtr++;
        counter++;

        // build the image bottom-up
        if ( counter % size.width() == 0 ) {
            outPtr -= size.width() * 2;
        }
    };
    view.forEach( lambda );
} // rawView2QImage

#include "CartaLib/PixelPipeline/Id2d.h"

// converter capable of computing clips

class RawView2QImageConverter2
{
    CLASS_BOILERPLATE( RawView2QImageConverter2 );

public:

    typedef double Scalar;

    RawView2QImageConverter2();

    /// what size cache should we use (in bytes)
    void
    setCacheSize( int64_t size );

    /// \warning the pointer is kept for invocation of go(). Make sure it's valid
    /// during a call to go()!
    /// @param id string unique for the view, as it will be used by caching
    Me &
    setView( NdArray::RawViewInterface * rawView, QString id = "" );

    /// compute clips using the current view (set by setView())
    /// @param clip for example 0.95 means 95%
    void
    computeClips( double clip );

    /// manual setting of clips
    void
    setClips( double min, double max );

    /// set the pixel pipeline (uncached)
    void
    setPixelPipeline( Carta::Lib::PixelPipeline::Id2NormQRgb * pipeline );

    /// destructor
    ~RawView2QImageConverter2();

    const QImage &
    go( int frame, bool recomputeClip = true );

protected:

    struct CachedImage;
    QImage m_qImage;
    Carta::Lib::IColormapScalar::SharedPtr m_cmap;
    NdArray::RawViewInterface * m_rawView = nullptr;
    double m_autoClip = 0.95;

    QCache < int, CachedImage > m_cache;
    double m_clip1 = 1.0 / 0.0, m_clip2 = 0.0;
};

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
    setPipelineCacheSize( int64_t size );

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
    setColormap( Lib::PixelPipeline::IColormap::SharedPtr colormap )
    ;

    /// do the actual conversion with the previously set parameters
    void convert(QImage & img);

private:
    /// pointer to the view
    NdArray::RawViewInterface * m_rawView = nullptr;
    NdArray::TypedView<double>::UniquePtr m_typedView;
    /// cache for images
    QCache < QString, QImage > m_imageCache;
    /// unoptimized pipeline
    Lib::PixelPipeline::CustomizablePipeline::UniquePtr m_customPipeline;
    /// optimized pipeline
    Lib::PixelPipeline::CachedPipeline::UniquePtr m_cachedPipeline;

};
}
}
