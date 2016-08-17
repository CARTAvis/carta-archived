#include "ProfileRenderService.h"
#include "ProfileRenderWorker.h"
#include "ProfileRenderThread.h"
#include "Data/Image/Layer.h"
#include "Data/Region/Region.h"
#include "CartaLib/Hooks/ProfileHook.h"

namespace Carta {
namespace Data {

ProfileRenderService::ProfileRenderService( QObject * parent ) :
        QObject( parent ),
        m_worker( nullptr),
        m_renderThread( nullptr ){
    m_renderQueued = false;
}


bool ProfileRenderService::renderProfile(std::shared_ptr<Layer> layer,
        std::shared_ptr<Region> region, const Carta::Lib::ProfileInfo& profInfo,
        bool createNew ){
    bool profileRender = true;
    if ( layer && !m_renderQueued ){
        RenderRequest request;
        request.m_layer = layer;
        request.m_region = region;
        request.m_profileInfo = profInfo;
        request.m_createNew = createNew;
        m_requests.enqueue( request );
        _scheduleRender( layer, region, profInfo );
    }
    else {
        profileRender = false;
    }
    return profileRender;
}


void ProfileRenderService::_scheduleRender( std::shared_ptr<Layer> layer,
        std::shared_ptr<Region> region, const Carta::Lib::ProfileInfo& profInfo){
    if ( m_renderQueued ) {
        return;
    }
    m_renderQueued = true;


    //Create a worker if we don't have one.
    if ( !m_worker ){
        m_worker = new ProfileRenderWorker();
    }

    if ( !m_renderThread ){
        m_renderThread = new ProfileRenderThread();
        connect( m_renderThread, SIGNAL(finished()),
                this, SLOT( _postResult()));
    }
    std::shared_ptr<Carta::Lib::Regions::RegionBase> regionInfo(nullptr);
    if ( region ){
        regionInfo = region->getInfo();
    }
    std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource = layer->_getImage();
    m_worker->setParameters( dataSource, regionInfo, profInfo );
    int pid = m_worker->computeProfile();
    if ( pid != -1 ){
        m_renderThread->setFileDescriptor( pid );
        m_renderThread->start();
    }
    else {
        m_renderQueued = false;
    }
}

void ProfileRenderService::_postResult(  ){
    Lib::Hooks::ProfileResult result = m_renderThread->getResult();
    RenderRequest request = m_requests.dequeue();
    emit profileResult(result, request.m_layer, request.m_region, request.m_createNew );
    m_renderQueued = false;
    if ( m_requests.size() > 0 ){
        RenderRequest& head = m_requests.head();
         _scheduleRender( head.m_layer,
                head.m_region, head.m_profileInfo );
    }
}


ProfileRenderService::~ProfileRenderService(){
    delete m_worker;
    delete m_renderThread;
}
}
}

