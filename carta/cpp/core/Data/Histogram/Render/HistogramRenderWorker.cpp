#include "HistogramRenderWorker.h"
#include "Data/Util.h"
#include "Globals.h"
#include "PluginManager.h"
#include "CartaLib/Hooks/Histogram.h"
#include "CartaLib/Hooks/HistogramResult.h"
#include <QFile>
#include <QDataStream>
#include <string.h>
#include <unistd.h>

namespace Carta
{
namespace Data
{

HistogramRenderWorker::HistogramRenderWorker(){
}


void HistogramRenderWorker::setParameters( const HistogramRenderRequest& request ){
    m_binCount = request.getBinCount();
    m_minChannel = request.getChannelMin();
    m_maxChannel = request.getChannelMax();
    m_minFrequency = request.getFrequencyMin();
    m_maxFrequency = request.getFrequencyMax();
    m_rangeUnits = request.getRangeUnits();
    m_minIntensity = request.getIntensityMin();
    m_maxIntensity = request.getIntensityMax();
    m_fileName = request.getFileName();

    m_dataSource = request.getImage();
    m_region = request.getRegion();
    m_regionId = request.getRegionId();

}


int HistogramRenderWorker::computeHist(){
    //Note:  we compute the histogram in a separate process because casacore tables
    //cannot be accessed by different threads at the same time.
    int histogramPipes [2];
    if (pipe (histogramPipes)){
        qDebug() << "*** HistogramRenderWorker::run: pipe creation failed: " << strerror (errno);
        return -1;
    }

    int pid = fork ();
    if (pid == -1){
        // Failure
        qDebug() << "*** HistogramRenderWorker::run: fork failed: " << strerror (errno);
        return -1;

    }
    else if (pid != 0){
        // The original process comes here.
        close (histogramPipes [1]); // close write end of the pipe
        return histogramPipes [0]; // return the read end of the pipe
    }

    // We're our own process
    // Close the read end of the pipe
    close( histogramPipes[0] );
    auto result = Globals::instance()-> pluginManager()
                          -> prepare <Carta::Lib::Hooks::HistogramHook>(m_dataSource, m_binCount,
                                  m_minChannel, m_maxChannel, m_minFrequency, m_maxFrequency, m_rangeUnits,
                                  m_minIntensity, m_maxIntensity, m_region, m_regionId );
    auto lam = [=] ( const Carta::Lib::Hooks::HistogramResult &data ) {
        m_result = data;
    };
    try {
        result.forEach( lam );
    }
    catch( char*& error ){
        qDebug() << "HistogramRenderWorker::run: caught error: " << error;
        m_result.setName( Util::ERROR +": "+QString(error) );
    }

    QFile file;
    if ( !file.open( histogramPipes[1], QIODevice::WriteOnly, QFileDevice::AutoCloseHandle ) ){
        qDebug() << "Could not write histogram results";
        close( histogramPipes[1] );
        exit(0);
        return 0;
    }
    QDataStream dataStream( &file );
    dataStream << m_result;
    file.close();
    exit(0);
    return 0;
}


HistogramRenderWorker::~HistogramRenderWorker(){
}
}
}

