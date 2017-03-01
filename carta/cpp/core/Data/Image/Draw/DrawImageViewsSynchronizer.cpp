#include "DrawImageViewsSynchronizer.h"
#include "DrawStackSynchronizer.h"

#include <QDebug>
#include <QThread>
#include <QCoreApplication>

namespace Carta {

namespace Data {

DrawImageViewsSynchronizer::DrawImageViewsSynchronizer( QObject* parent)
        : QObject( parent ),
          m_drawMain( nullptr ),
          m_drawContext( nullptr ),
          m_drawZoom( nullptr ){
    m_busy = false;
}


void DrawImageViewsSynchronizer::_doneMain( bool /*drawn*/){
    qDebug()<<"grimmer render-doneMain";
    m_busy = false;
    _startNextDraw();
}

void DrawImageViewsSynchronizer::_doneContext( bool /*drawn*/ ){
    m_busy = false;
    _startNextDraw();
}

void DrawImageViewsSynchronizer::_doneZoom( bool /*drawn*/){
    m_busy = false;
    _startNextDraw();
}

bool DrawImageViewsSynchronizer::isContextView() const {
    bool contextView = false;
    if ( m_drawContext ){
        contextView = true;
    }
    return contextView;
}

bool DrawImageViewsSynchronizer::_isRequested( const std::shared_ptr<RenderRequest>& request ) const {
	bool requested = false;
	int requestCount = m_requests.size();
	for ( int i = 0; i < requestCount; i++ ){
		if ( m_requests[i]->operator==( *request.get() ) ){
			requested = true;
			break;
		}
	}
	return requested;
}

bool DrawImageViewsSynchronizer::isZoomView() const {
    bool zoomView = false;
    if ( m_drawZoom ){
        zoomView = true;
    }
    return zoomView;
}


void DrawImageViewsSynchronizer::render( const std::shared_ptr<RenderRequest>& request ){
    qDebug()<<"grimmer render2, index:"<<request->m_frames[2]<<"."<<( request.get() );

	if ( !_isRequested( request) ){

        qDebug()<<"grimmer render3";

//        const bool isGuiThread =
//            QThread::currentThread() == QCoreApplication::instance()->thread();

//        if (isGuiThread){

//            qDebug()<<"in the gui thread";
//        }

//        QString tID = QString::number((long long)QThread::currentThread(), 16);
//        QString tID2 = QString::number((long long)QCoreApplication::instance()->thread(), 16);

		m_requests.enqueue( request );
		if ( m_busy ){
            qDebug()<<"grimmer render-busy";
            // _doneMain 完才會變成false
			return;
		}

		//Store the data & request.
		_startNextDraw();
    } else {

        // 一般同一個index的前一個會進來deque後後一個才會進來
        qDebug()<<"grimmer render2, already index:"<<request->m_frames[2]<<"."<<( request.get() );
    }
}

void DrawImageViewsSynchronizer::setViewDraw( std::shared_ptr<DrawStackSynchronizer> stackDraw ){
    m_drawMain = stackDraw;
    if ( m_drawMain.get() ){
        connect ( m_drawMain.get(), SIGNAL( done(bool) ), this, SLOT(_doneMain(bool)));
    }
}

void DrawImageViewsSynchronizer::setViewDrawContext( std::shared_ptr<DrawStackSynchronizer> stackDraw ){
    m_drawContext = stackDraw;
    if ( m_drawContext.get()){
        connect( m_drawContext.get(), SIGNAL( done(bool)), this, SLOT( _doneContext(bool)));
    }
}

void DrawImageViewsSynchronizer::setViewDrawZoom( std::shared_ptr<DrawStackSynchronizer> stackDraw ){
    m_drawZoom = stackDraw;
    if ( m_drawZoom.get() ){
        connect( m_drawZoom.get(), SIGNAL( done(bool)), this, SLOT( _doneZoom(bool)));
    }
}

void DrawImageViewsSynchronizer::_startNextDraw(){
    if ( m_requests.size() > 0 ){
        m_busy = true;
        std::shared_ptr<RenderRequest> request = m_requests.dequeue();
        // 有時候前一個還沒dequeu.
        qDebug()<<"grimmer render2, dequeu index:"<<request->m_frames[2]<<"."<<( request.get() );

        if ( request->isRequestMain() ){
            //Start the main renderer
            request->setOutputSize( m_drawMain->getClientSize() );
            m_drawMain-> _render( request);
        }
        else if ( request->isRequestContext() ){
            request->setOutputSize( m_drawContext->getClientSize() );
            m_drawContext-> _render(request);
        }
        else if ( request->isRequestZoom() ){
            request->setOutputSize( m_drawZoom->getClientSize() );
            m_drawZoom->_render(request);
        }
    }
}


DrawImageViewsSynchronizer::~DrawImageViewsSynchronizer(){

}
}
}
