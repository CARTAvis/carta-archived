#include "ProfileRenderService.h"
#include "ProfileRenderWorker.h"
#include "ProfileRenderThread.h"
#include "CartaLib/Hooks/ProfileHook.h"

namespace Carta {
namespace Data {

ProfileRenderService::ProfileRenderService( QObject * parent ) :
        QObject( parent ),
        m_worker( nullptr),
        m_renderThread( nullptr ){
    m_renderQueued = false;
}


bool ProfileRenderService::renderProfile(std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
        Carta::Lib::RegionInfo& regionInfo, Carta::Lib::ProfileInfo& profInfo,
        int curveIndex, const QString& layerName, bool createNew ){
    bool profileRender = true;
    if ( dataSource && !m_renderQueued ){
        RenderRequest request;
        request.m_image = dataSource;
        request.m_regionInfo = regionInfo;
        request.m_profileInfo = profInfo;
        request.m_curveIndex = curveIndex;
        request.m_layerName = layerName;
        request.m_createNew = createNew;
        m_requests.enqueue( request );
        _scheduleRender( dataSource, regionInfo, profInfo );
    }
    else {
        profileRender = false;
    }
    return profileRender;
}


void ProfileRenderService::_scheduleRender( std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
        Carta::Lib::RegionInfo& regionInfo, Carta::Lib::ProfileInfo& profInfo){
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
    bool paramsChanged = m_worker->setParameters( dataSource, regionInfo, profInfo );
    if ( paramsChanged ){

        int pid = m_worker->computeProfile();
        if ( pid != -1 ){
            m_renderThread->setFileDescriptor( pid );
            m_renderThread->start();
        }
        else {
            qDebug() << "Bad file descriptor: "<<pid;
            m_renderQueued = false;
        }
    }
    else {
        m_renderQueued = false;
    }
}

void ProfileRenderService::_postResult(  ){
    Lib::Hooks::ProfileResult result = m_renderThread->getResult();
    RenderRequest request = m_requests.dequeue();
    emit profileResult(result, request.m_curveIndex, request.m_layerName,
            request.m_createNew, request.m_image );
    m_renderQueued = false;
    if ( m_requests.size() > 0 ){
        RenderRequest& head = m_requests.head();
         _scheduleRender( head.m_image,
                head.m_regionInfo, head.m_profileInfo );
    }
}


ProfileRenderService::~ProfileRenderService(){
    delete m_worker;
    delete m_renderThread;
}
}
}

