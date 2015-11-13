#include "ImageSaveService.h"
#include "ImageRenderService.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"

namespace Carta
{
namespace Core
{
namespace ImageSaveService
{

ImageSaveService::ImageSaveService( QString savename,
        std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> m_pixelPipeline,
        QObject * parent ) : QObject( parent ),
                m_inputView( nullptr )
{
    m_outputFilename = savename;
    m_pixelPipelineCopy = m_pixelPipeline;
    m_renderService.reset( new Carta::Core::ImageRenderService::Service());
    connect( m_renderService.get(), & Carta::Core::ImageRenderService::Service::done,
             this, & ImageSaveService::_saveFullImageCB );
}

ImageSaveService::~ImageSaveService()
{ 
    // if( m_renderService) {
    //     delete m_renderService;
    // }
}

void ImageSaveService::setZoom( double zoom ){
    CARTA_ASSERT( m_renderService);
    m_renderService->setZoom( zoom );
}

void ImageSaveService::setOutputSize( QSize size ){
    m_outputSize = size;
}


void ImageSaveService::setInputView( std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view,
        const QString& viewId ){
    m_inputView = view;
    m_inputViewId = viewId;
}

void ImageSaveService::setDisplayShape( int dimAxis1, int dimAxis2 ){
    CARTA_ASSERT( dimAxis1 >= 0 && dimAxis2 >= 0 );
    m_inputXFrames = dimAxis1;
    m_inputYFrames = dimAxis2;
}

void ImageSaveService::setAspectRatioMode( Qt::AspectRatioMode mode ){
    m_aspectRatioMode = mode;
}

bool ImageSaveService::saveFullImage(){
    bool dataPrepared = _prepareData( );
    if ( dataPrepared ){
        CARTA_ASSERT( m_renderService);
        m_renderService->render( 0 );
    }
    return dataPrepared;
}

bool ImageSaveService::_prepareData( ){
    bool dataPrepared = false;
    if ( m_inputView ){
        CARTA_ASSERT( m_renderService);
        m_renderService-> setPixelPipeline( m_pixelPipelineCopy, m_pixelPipelineCopy-> cacheId());
        m_renderService-> setInputView( m_inputView, m_inputViewId );

        double zoom = m_renderService->zoom();
        QSize outputSize = QSize( zoom * m_inputXFrames, zoom * m_inputYFrames );
        qDebug() << "IMS outputSize=" << outputSize;
        qDebug() << "IMS inputframes" << m_inputXFrames << m_inputYFrames;
        qDebug() << "IMS zoom" << zoom;
        m_renderService->setOutputSize( outputSize);
//        m_renderService->setOutputSize( QSize( zoom * m_inputXFrames, zoom * m_inputYFrames ) );
        m_renderService->setPan( { m_inputXFrames / 2.0 - 0.5, m_inputYFrames / 2.0 - 0.5 });
        dataPrepared = true;
    }
    return dataPrepared;
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
}


}
}
}
