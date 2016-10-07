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


bool HistogramRenderService::renderHistogram( const HistogramRenderRequest& request ){
	bool histogramRender = true;
	if ( request.getImage() ){
		if ( ! m_requests.contains( request ) ){
			m_requests.enqueue( request );
			_scheduleRender( request );
		}
	}
	else {
		histogramRender = false;
	}
	return histogramRender;
}


void HistogramRenderService::_scheduleRender( const HistogramRenderRequest& request ){
	if ( m_renderQueued ) {
		return;
	}
	m_renderQueued = true;
	if ( !m_worker ){
		m_worker = new HistogramRenderWorker();
	}
	m_worker->setParameters( request );
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
		m_renderQueued = false;
	}

}

void HistogramRenderService::_postResult( ){
	Carta::Lib::Hooks::HistogramResult result = m_renderThread->getResult();
	m_requests.dequeue();
	emit histogramResult( result );
	m_renderQueued = false;
	if ( m_requests.size() > 0 ){
		HistogramRenderRequest& head = m_requests.head();
		_scheduleRender( head );
	}
}


HistogramRenderService::~HistogramRenderService(){
    if ( m_renderThread ){
    	m_renderThread->wait();
    	delete m_renderThread;
    }
    delete m_worker;
}
}
}

