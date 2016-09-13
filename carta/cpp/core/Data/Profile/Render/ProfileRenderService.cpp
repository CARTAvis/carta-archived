#include "ProfileRenderService.h"
#include "ProfileRenderWorker.h"
#include "ProfileRenderThread.h"
#include "ProfileRenderRequest.h"
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
    ProfileRenderRequest request( layer, region, profInfo, createNew );
    if ( layer ){
    	if ( ! m_requests.contains( request ) ){
    		m_requests.enqueue( request );
    		_scheduleRender( layer, region, profInfo );
    	}
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
        regionInfo = region->getModel();
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
    ProfileRenderRequest request = m_requests.dequeue();
    emit profileResult(result, request.getLayer(), request.getRegion(), request.isCreateNew() );
    m_renderQueued = false;
    if ( m_requests.size() > 0 ){
        ProfileRenderRequest& head = m_requests.head();
         _scheduleRender( head.getLayer(), head.getRegion(), head.getProfileInfo() );
    }
}


ProfileRenderService::~ProfileRenderService(){
    delete m_worker;
    delete m_renderThread;
}
}
}

