#include "HistogramRenderThread.h"
#include "Data/Util.h"
#include "CartaLib/Hooks/HistogramResult.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

namespace Carta
{
namespace Data
{

HistogramRenderThread::HistogramRenderThread( int pipeFd, QObject* parent ):
    QThread( parent ){
    m_fileDescriptor = pipeFd;
}

Carta::Lib::Hooks::HistogramResult HistogramRenderThread::getResult() const {
    return m_result;
}


void HistogramRenderThread::run(){
   QFile file;
   if ( !file.open( m_fileDescriptor, QIODevice::ReadOnly, QFileDevice::AutoCloseHandle ) ){
       QString errorStr(Util::ERROR + ": Could not read histogram results");
       qDebug() << errorStr;
       m_result.setName( errorStr );
   }
   else {
       QDataStream dataStream( & file );
       dataStream >> m_result;
       file.close();
   }
}

void HistogramRenderThread::setFileDescriptor( int fileDescriptor ){
    m_fileDescriptor = fileDescriptor;
}


HistogramRenderThread::~HistogramRenderThread(){
}
}
}

