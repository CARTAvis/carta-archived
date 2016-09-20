#include "DrawImageViewsSynchronizer.h"
#include "DrawStackSynchronizer.h"

#include <QDebug>

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
	if ( !_isRequested( request) ){
		m_requests.enqueue( request );
		if ( m_busy ){
			return;
		}

		//Store the data & request.
		_startNextDraw();
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
