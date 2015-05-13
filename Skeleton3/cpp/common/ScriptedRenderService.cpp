#include "ScriptedRenderService.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{

ScriptedRenderService::ScriptedRenderService( QString savename, std::shared_ptr<Image::ImageInterface> &m_image, std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> &m_pixelPipeline, QString filename, QObject * parent ) : QObject( parent )
{
    m_outputFilename = savename;
    m_imageCopy = m_image;
    m_pixelPipelineCopy = m_pixelPipeline;
    m_inputFilename = filename;
    m_renderService = new Carta::Core::ImageRenderService::Service();
    connect( m_renderService, & Carta::Core::ImageRenderService::Service::done,
             this, & ScriptedRenderService::_readyToSave );
}

ScriptedRenderService::~ScriptedRenderService()
{ }

void ScriptedRenderService::setZoom( double zoom ){
    m_renderService->setZoom( zoom );
}

bool ScriptedRenderService::saveFullImage(){
    _prepareData( 0, 0.0, 1.0 );
    m_renderService->render( 0 );
    return false;
}

void ScriptedRenderService::_prepareData( int frameIndex, double minClipPercentile, double maxClipPercentile ){

    if ( frameIndex < 0 ) {
        frameIndex = 0;
    }
    if ( m_imageCopy-> dims().size() <= 2 ) {
        frameIndex = 0;
    }
    else {
        frameIndex = clamp( frameIndex, 0, m_imageCopy-> dims()[2] - 1 );
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
    m_renderService->setPan( { m_imageCopy->dims()[0] / 2.0, m_imageCopy->dims()[1] / 2.0 });
}

void ScriptedRenderService::_readyToSave( QImage img ){
    img.save( m_outputFilename );
}


}
}
}
