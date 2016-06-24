#include "ProfileRenderThread.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

namespace Carta
{
namespace Data
{

ProfileRenderThread::ProfileRenderThread( QObject* parent ):
    QThread( parent ){
}

Carta::Lib::Hooks::ProfileResult ProfileRenderThread::getResult() const {
    return m_result;
}


void ProfileRenderThread::run(){
   QFile file;
   if ( !file.open( m_fileDescriptor, QIODevice::ReadOnly, QFileDevice::AutoCloseHandle ) ){
       QString errorStr("Could not read Profile results");
       m_result.setError( errorStr );
   }
   else {
       QDataStream dataStream( & file );
       dataStream >> m_result;
       file.close();
   }
}

void ProfileRenderThread::setFileDescriptor( int fileDescriptor ){
    m_fileDescriptor = fileDescriptor;
}



ProfileRenderThread::~ProfileRenderThread(){
}
}
}

