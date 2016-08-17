#include "ProfileRenderWorker.h"
#include "Globals.h"
#include "PluginManager.h"
#include "CartaLib/Hooks/ProfileHook.h"
#include <QFile>
#include <QDataStream>
#include <string.h>
#include <unistd.h>

namespace Carta
{
namespace Data
{

ProfileRenderWorker::ProfileRenderWorker(){
}


bool ProfileRenderWorker::setParameters(std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
       std::shared_ptr<Carta::Lib::Regions::RegionBase> regionInfo,
       const Carta::Lib::ProfileInfo& profInfo  ){
    bool paramsChanged = false;
    if ( m_regionInfo != regionInfo ){
        m_regionInfo = regionInfo;
        paramsChanged = true;
    }
    if ( m_profileInfo != profInfo ){
        m_profileInfo = profInfo;
        paramsChanged = true;
    }
    //if ( m_dataSource.get() != dataSource.get() ){
        m_dataSource = dataSource;
        //paramsChanged = true;
    //}
    return paramsChanged;
}


int ProfileRenderWorker::computeProfile(){
    //Note:  we compute the Profile in a separate process because casacore tables
    //cannot be accessed by different threads at the same time.
    int ProfilePipes [2];
    if (pipe (ProfilePipes)){
        qDebug() << "*** ProfileRenderWorker::run: pipe creation failed: " << strerror (errno);
        return -1;
    }

    int pid = fork ();
    if (pid == -1){
        // Failure
        qDebug() << "*** ProfileRenderWorker::run: fork failed: " << strerror (errno);
        return -1;

    }
    else if (pid != 0){
        // The original process comes here.
        close (ProfilePipes [1]); // close write end of the pipe
        return ProfilePipes [0]; // return the read end of the pipe
    }

    // We're our own process
    auto result = Globals::instance()-> pluginManager()
                          -> prepare <Carta::Lib::Hooks::ProfileHook>(m_dataSource, m_regionInfo,
                                  m_profileInfo);
    auto lam = [=] ( const Carta::Lib::Hooks::ProfileResult &data ) {
        m_result = data;
    };
    try {
        result.forEach( lam );
    }
    catch( char*& error ){
        qDebug() << "ProfileRenderWorker::run: caught error: " << error;
        m_result.setError( QString(error) );
    }

    QFile file;
    if ( !file.open( ProfilePipes[1], QIODevice::WriteOnly, QFileDevice::AutoCloseHandle ) ){
        qDebug() << "Could not write Profile results";
        return 0;
    }
    QDataStream dataStream( &file );
    dataStream << m_result;
    file.close();
    exit(0);
    return 0;
}


ProfileRenderWorker::~ProfileRenderWorker(){
}
}
}

