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
///
/// \warning deprecated in favor of ImageRenderService

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
