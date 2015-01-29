/**
 *
 **/

#include "ImageRenderService.h"
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
void
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
    QRgb nanColor = qRgb( 255, 0, 0);
    auto lambda = [&] ( const Scalar & ival )
    {
        if( Q_LIKELY(! std::isnan( ival))) {
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


Service::Service( QObject * parent ) : QObject( parent )
{
    // hook up the internal schedule helper signal to the scheduleJob slot, using
    // queued connection
    auto conn = connect( this, & Service::internalRenderSignal,
             this, & Service::internalRenderSlot,
             Qt::QueuedConnection);
    qDebug() << "ivc connect" << conn;
}

Service::~Service()
{ }

void Service::internalRenderSlot( JobId jobId)
{
    if( ! m_inputView) {
        qCritical() << "input view not set";
        return;
    }

    static int counter = 0;
    counter ++;

    // render the frame
    if( m_frameImage.isNull()) {
        if( m_pixelPipelineRaw) {
            Lib::PixelPipeline::CachedPipeline < true > cachedPixPipe;
            double clipMin, clipMax;
            m_pixelPipelineRaw-> getClips( clipMin, clipMax);
            cachedPixPipe.cache( * m_pixelPipelineRaw, 1000, clipMin, clipMax );

            ::rawView2QImage2( m_inputView.get(), cachedPixPipe, m_frameImage );
        }
    }


    QSize outputSize = m_outputSize;
    if( m_outputSize.isEmpty()) {
        qWarning() << "output size empty";
        outputSize = QSize( 10, 10);
    }

    QImage img( outputSize, QImage::Format_ARGB32);
    img.fill( QColor( "red"));
    QPainter p( & img);
    p.drawImage( 0, 0, m_frameImage);
    p.setPen( QColor( "yellow"));
    p.drawText( img.rect(), Qt::AlignCenter, QString::number(counter));
    emit done( img, jobId);
}

}
}
}
