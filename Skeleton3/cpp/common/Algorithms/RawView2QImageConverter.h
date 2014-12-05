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
/// \brief rawView2QImage
/// \tparam Pipeline
/// \param m_rawView
/// \param pipe
/// \param m_qImage
///
template < class Pipeline >
void
rawView2QImage( NdArray::RawViewInterface * m_rawView, Pipeline & pipe, QImage & m_qImage )
{
    typedef double Scalar;
    QSize size( m_rawView->dims()[0], m_rawView->dims()[1] );

    if ( m_qImage.format() != QImage::Format_ARGB32 ||
         m_qImage.size() != size ) {
        m_qImage = QImage( size, QImage::Format_ARGB32 );
    }

    // construct image using clips (clipd, clipinv)
    auto bytesPerLine = m_qImage.bytesPerLine();
    CARTA_ASSERT( bytesPerLine == size.width() * 4 );

    // start with a pointer to the beginning of last row (we are constructing image
    // bottom-up)
    QRgb * outPtr = reinterpret_cast < QRgb * > (
        m_qImage.bits() + size.width() * ( size.height() - 1 ) * 4 );

    // make a double view
    NdArray::TypedView < Scalar > view( m_rawView, false );

    /// @todo for more efficiency we should switch to the higher performance view apis
    /// and apply some basic openmp/cilk
    int64_t counter = 0;
    auto lambda = [&] ( const Scalar & ival )
    {
        pipe.convert( ival, * outPtr);
        outPtr++;
        counter++;
        // build the image bottom-up
        if ( counter % size.width() == 0 ) {
            outPtr -= size.width() * 2;
        }
    };
    view.forEach( lambda );
} // rawView2QImage
