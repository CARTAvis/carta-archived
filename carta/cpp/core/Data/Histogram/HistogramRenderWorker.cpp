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


bool HistogramRenderWorker::setParameters(std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
        int binCount, int minChannel, int maxChannel, double minFrequency, double maxFrequency,
        const QString& rangeUnits, double minIntensity, double maxIntensity,
        const QString& fileName ){
    bool paramsChanged = false;
    if ( m_binCount != binCount ){
        m_binCount = binCount;
        paramsChanged = true;
    }
    if ( m_minChannel != minChannel ){
        m_minChannel = minChannel;
        paramsChanged = true;
    }
    if ( m_maxChannel != maxChannel ){
        m_maxChannel = maxChannel;
        paramsChanged = true;
    }
    if ( m_minFrequency != minFrequency ){
        m_minFrequency = minFrequency;
        paramsChanged = true;
    }
    if ( m_maxFrequency != maxFrequency ){
        m_maxFrequency = maxFrequency;
        paramsChanged = true;
    }
    if ( m_rangeUnits != rangeUnits ){
        m_rangeUnits = rangeUnits;
        paramsChanged = true;
    }
    if ( m_minIntensity != minIntensity ){
        m_minIntensity = minIntensity;
        paramsChanged = true;
    }
    if ( m_maxIntensity != maxIntensity ){
        m_maxIntensity = maxIntensity;
        paramsChanged = true;
    }
    if ( m_fileName != fileName){
        m_fileName = fileName;
        paramsChanged = true;
    }
    m_dataSource = dataSource;
    return paramsChanged;
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
    auto result = Globals::instance()-> pluginManager()
                          -> prepare <Carta::Lib::Hooks::HistogramHook>(m_dataSource, m_binCount,
                                  m_minChannel, m_maxChannel, m_minFrequency, m_maxFrequency, m_rangeUnits,
                                  m_minIntensity, m_maxIntensity);
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

