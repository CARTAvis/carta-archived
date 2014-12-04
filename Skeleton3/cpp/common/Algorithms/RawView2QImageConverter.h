/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include "CartaLib/IColormapScalar.h"
#include "CartaLib/IImage.h"
#include <QImage>
#include <QCache>

/// algorithm for converting an instance of image interface to qimage
class RawView2QImageConverter
{
public:

    typedef RawView2QImageConverter & Me;
    typedef double                    Scalar;

    RawView2QImageConverter();

    ~RawView2QImageConverter();

    /// \warning the pointer is kept for invocation of go(). Make sure it's valid
    /// during a call to go()!
    Me &
    setView( NdArray::RawViewInterface * rawView );

    Me &
    setFrame( int frame );

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

//    std::unique_ptr< QCache<int,CachedImage> > m_cache;
    QCache < int, CachedImage > m_cache;
    double m_clip1 = 1.0 / 0.0, m_clip2 = 0.0;
};

/// algorithm for converting an instance of image interface to qimage
/// using the pixel pipeline
class RawView2QImageConverter2
{
    CLASS_BOILERPLATE( RawView2QImageConverter2 );

public:

    typedef double Scalar;

    RawView2QImageConverter2();

    ~RawView2QImageConverter2();

    /// \warning the pointer is kept for invocation of go(). Make sure it's valid
    /// during a call to go()!
    Me &
    setView( NdArray::RawViewInterface * rawView );

    Me &
    setFrame( int frame );

    Me &
    setAutoClip( double val );

    Me &
    setColormap( Carta::Lib::IColormapScalar::SharedPtr cmap );

    const QImage &
    go( int frame, bool recomputeClip = true );

protected:

    struct CachedImage;
    QImage m_qImage;
    NdArray::RawViewInterface * m_rawView = nullptr;
    double m_autoClip = 0.95;
    QCache < int, CachedImage > m_cache;
    double m_clip1 = 1.0 / 0.0, m_clip2 = 0.0;
};
