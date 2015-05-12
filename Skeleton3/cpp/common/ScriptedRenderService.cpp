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
    // hook up the internal schedule helper signal to the scheduleJob slot, using
    // queued connection
//    auto conn = connect( this, & Service::internalRenderSignal,
//                         this, & Service::internalRenderSlot,
//                         Qt::QueuedConnection );
//
//    m_frameCache.setMaxCost( 1 * 1024 * 1024 * 1024); // 1 gig
    m_fileName = filename;
    m_imageCopy = m_image;
    m_pixelPipelineCopy = m_pixelPipeline;
    qDebug() << "(JT) ScriptedRenderService::ScriptedRenderService() m_imageCopy-> dims().size() =" << m_imageCopy->dims().size();
    m_renderService = new Carta::Core::ImageRenderService::Service();
    connect( m_renderService, & Carta::Core::ImageRenderService::Service::done,
             this, & ScriptedRenderService::_renderingDone);
    m_savename = savename;
}

ScriptedRenderService::~ScriptedRenderService()
{ }

bool ScriptedRenderService::saveFullImage(){
    qDebug() << "(JT) ScriptedRenderService::saveFullImage()";
    qDebug() << "(JT) m_savename =" << m_savename;
    setInputView( 0, 0.0, 1.0 );
    m_renderService->render( 0 );
    return false;
}

void ScriptedRenderService::setInputView( int frameIndex, double minClipPercentile, double maxClipPercentile ){

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

    //Update the clip values
//    _updateClips( view, frameIndex, minClipPercentile, maxClipPercentile );

    m_renderService-> setPixelPipeline( m_pixelPipelineCopy, m_pixelPipelineCopy-> cacheId());

    // tell the render service to render this job
    QString argStr = QString( "%1//%2").arg(m_fileName).arg(frameIndex);
    m_renderService-> setInputView( view, argStr);
}

void ScriptedRenderService::_renderingDone( QImage img ){
    qDebug() << "(JT) ScriptedRenderService::_renderingDone()";
    img.save( m_savename );
}


}
}
}
