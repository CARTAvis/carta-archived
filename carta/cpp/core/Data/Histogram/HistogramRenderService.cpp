#include "HistogramRenderService.h"
#include "HistogramRenderWorker.h"
#include "HistogramRenderThread.h"
#include "CartaLib/Hooks/Histogram.h"
#include "Data/Util.h"

namespace Carta {
namespace Data {

HistogramRenderService::HistogramRenderService( QObject * parent ) :
        QObject( parent ),
        m_worker( nullptr),
        m_renderThread( nullptr ){
    m_renderQueued = false;
}

Carta::Lib::Hooks::HistogramResult HistogramRenderService::getResult() const {
    return m_result;
}


bool HistogramRenderService::renderHistogram(std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
        int binCount, int minChannel, int maxChannel, double minFrequency, double maxFrequency,
        const QString& rangeUnits, double minIntensity, double maxIntensity,
        const QString& fileName ){
    bool histogramRender = true;
    if ( dataSource ){
        _scheduleRender( dataSource, binCount, minChannel, maxChannel, minFrequency, maxFrequency,
                rangeUnits, minIntensity, maxIntensity, fileName );
    }
    else {
        histogramRender = false;
    }
    return histogramRender;
}


void HistogramRenderService::_scheduleRender( std::shared_ptr<Carta::Lib::Image::ImageInterface> dataSource,
        int binCount, int minChannel, int maxChannel, double minFrequency, double maxFrequency,
        const QString& rangeUnits, double minIntensity, double maxIntensity, const QString& fileName ){
    if ( m_renderQueued ) {
        return;
    }
    if ( !m_worker ){
        m_worker = new HistogramRenderWorker();
    }
    bool paramsChanged = m_worker->setParameters( dataSource, binCount, minChannel, maxChannel, minFrequency, maxFrequency,
               rangeUnits, minIntensity, maxIntensity, fileName );
    if ( paramsChanged ){
        m_renderQueued = true;
        int pid = m_worker->computeHist();
        if ( pid != -1 ){
            if ( !m_renderThread ){
                m_renderThread = new HistogramRenderThread( pid );
                connect( m_renderThread, SIGNAL(finished()), this, SLOT( _postResult()));
            }
            else {
                m_renderThread->setFileDescriptor( pid );
            }
            m_renderThread->start();
        }
        else {
            qDebug() << "Bad file descriptor: "<<pid;
        }
    }
}

void HistogramRenderService::_postResult( ){
    m_result = m_renderThread->getResult();
    emit histogramResult( );
    m_renderQueued = false;
}


HistogramRenderService::~HistogramRenderService(){
    delete m_worker;
    delete m_renderThread;
}
}
}

