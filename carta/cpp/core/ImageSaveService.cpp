#include "ImageSaveService.h"
#include "ImageRenderService.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"

namespace Carta
{
namespace Core
{
namespace ImageSaveService
{

ImageSaveService::ImageSaveService( QString savename, std::shared_ptr<Image::ImageInterface> &m_image, std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> &m_pixelPipeline, QString filename, QObject * parent ) : QObject( parent )
{
    m_outputFilename = savename;
    m_imageCopy = m_image;
    m_pixelPipelineCopy = m_pixelPipeline;
    m_inputFilename = filename;
    m_renderService = new Carta::Core::ImageRenderService::Service();
    connect( m_renderService, & Carta::Core::ImageRenderService::Service::done,
             this, & ImageSaveService::_saveFullImageCB );
}

ImageSaveService::~ImageSaveService()
{ }

void ImageSaveService::setZoom( double zoom ){
    m_renderService->setZoom( zoom );
}

void ImageSaveService::setOutputSize( QSize size ){
    m_outputSize = size;
}

void ImageSaveService::setFrameIndex( int index ){
    m_frameIndex = index;
}

void ImageSaveService::setAspectRatioMode( Qt::AspectRatioMode mode ){
    m_aspectRatioMode = mode;
}

void ImageSaveService::saveFullImage(){
    _prepareData( m_frameIndex, 0.0, 1.0 );
    m_renderService->render( 0 );
}

/// \todo not all parameters are being used?
void ImageSaveService::_prepareData( int frameIndex, double /*minClipPercentile*/, double /*maxClipPercentile*/ ){

    if ( frameIndex < 0 ) {
        frameIndex = 0;
    }
    if ( m_imageCopy-> dims().size() <= 2 ) {
        frameIndex = 0;
    }
    else {
        frameIndex = Carta::Lib::clamp( frameIndex, 0, m_imageCopy-> dims()[2] - 1 );
    }

    // prepare slice description corresponding to the entire frame [:,:,frame,0,0,...0]
    auto frameSlice = SliceND().next();
    for ( size_t i = 2 ; i < m_imageCopy->dims().size() ; i++ ) {
        frameSlice.next().index( i == 2 ? frameIndex : 0 );
    }

    // get a view of the data using the slice description and make a shared pointer out of it
    NdArray::RawViewInterface::SharedPtr view( m_imageCopy-> getDataSlice( frameSlice ) );

    m_renderService-> setPixelPipeline( m_pixelPipelineCopy, m_pixelPipelineCopy-> cacheId());

    QString argStr = QString( "%1//%2").arg(m_inputFilename).arg(frameIndex);
    m_renderService-> setInputView( view, argStr);

    double zoom = m_renderService->zoom();
    m_renderService->setOutputSize( QSize( zoom * m_imageCopy->dims()[0], zoom * m_imageCopy->dims()[1] ) );
    m_renderService->setPan( { m_imageCopy->dims()[0] / 2.0 - 0.5, m_imageCopy->dims()[1] / 2.0 - 0.5 });
}

void ImageSaveService::_saveFullImageCB( QImage img ){
    QSize imgSize = img.size();
    qDebug() << "outputSize width="<<m_outputSize.width() << " height="<<m_outputSize.height();
    qDebug() << "img height="<<imgSize.height()<<" width="<<imgSize.width();
    qDebug() << "aspect="<<m_aspectRatioMode;
    QImage imgScaled = img.scaled( m_outputSize * m_renderService->zoom(), m_aspectRatioMode );
    QSize scaledSize = imgScaled.size();
    qDebug() << "scaled height="<<scaledSize.height()<<" width="<<scaledSize.width();
    bool result = imgScaled.save( m_outputFilename );
    emit saveImageResult( result );
    m_renderService->deleteLater();
}


}
}
}
