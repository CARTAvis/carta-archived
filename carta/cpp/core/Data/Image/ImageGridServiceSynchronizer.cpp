#include "ImageGridServiceSynchronizer.h"
#include "ImageRenderService.h"
#include "CartaLib/IWcsGridRenderService.h"

#include <QDebug>

namespace Carta {

namespace Data {

ImageGridServiceSynchronizer::ImageGridServiceSynchronizer( std::shared_ptr<Carta::Core::ImageRenderService::Service> imageRendererService,
            std::shared_ptr<Carta::Lib::IWcsGridRenderService> gridRendererService, QObject* parent)
        : QObject( parent ),
          m_irsJobId(-1),
          m_grsJobId(-1),
          m_jobId(-1),
          m_irs( nullptr ),
          m_grs( nullptr ){
    if ( ! connect( imageRendererService.get(), & Carta::Core::ImageRenderService::Service::done,
            this, & ImageGridServiceSynchronizer::irsDoneSlot ) ) {
        qCritical() << "Could not connect imageRenderService done slot";
    }
    if ( ! connect( gridRendererService.get(), & Carta::Lib::IWcsGridRenderService::done,
            this, & ImageGridServiceSynchronizer::wcsGridSlot ) ) {
        qCritical() << "Could not connect gridRenderService done slot";
    }

    m_irs = imageRendererService;
    m_grs = gridRendererService;
}

void ImageGridServiceSynchronizer::irsDoneSlot( QImage img, int64_t jobId ){
    // if this is not the expected job, do nothing
    if ( jobId != m_irsJobId ) { return; }
    m_irsDone = true;
    m_irsImage = img;
    if ( m_grsDone ) {
        emit done( m_irsImage, m_grsVGList, m_jobId );
    }
}

int64_t ImageGridServiceSynchronizer::start( int64_t jobId ){
    m_irsDone = false;
    m_grsDone = false;
    if ( jobId < 0 ) {
        m_jobId++;
    }
    else {
        m_jobId = jobId;
    }
    m_irsJobId = m_irs-> render();
    m_grsJobId = m_grs-> startRendering();
    return m_jobId;
}

void ImageGridServiceSynchronizer::wcsGridSlot( Carta::Lib::VectorGraphics::VGList vgList,
             int64_t jobId )
{
    // if this is not the expected job, do nothing
    if ( jobId != m_grsJobId ) { return; }
    m_grsDone = true;
    m_grsVGList = vgList;
    if ( m_irsDone ) {
        emit done( m_irsImage, m_grsVGList, m_jobId );
    }
}

ImageGridServiceSynchronizer::~ImageGridServiceSynchronizer(){

}
}
}
