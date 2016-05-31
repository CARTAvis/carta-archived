#include "DrawImageViewsSynchronizer.h"
#include "DrawStackSynchronizer.h"
#include "Data/Image/DataSource.h"

#include <QDebug>

namespace Carta {

namespace Data {

DrawImageViewsSynchronizer::DrawImageViewsSynchronizer( QObject* parent)
        : QObject( parent ),
          m_request( nullptr ),
          m_drawMain( nullptr ),
          m_drawContext( nullptr ),
          m_drawZoom( nullptr ){
}


void DrawImageViewsSynchronizer::_doneMain( bool drawn){
    m_doneMain = true;
    if ( drawn ){
        _startNextDraw();
    }
    else {
        //Don't proceed with the rest of the draw if the first
        //one didn't succeed.
        m_doneContext = true;
        m_doneZoom = true;
    }
}

void DrawImageViewsSynchronizer::_doneContext( bool drawn ){
    m_doneContext = true;
    if ( drawn ){
        _startNextDraw();
    }
    else {
        m_doneMain = true;
        m_doneZoom = true;
    }
}

void DrawImageViewsSynchronizer::_doneZoom( bool drawn){
    m_doneZoom = true;
    if ( drawn ){
        _startNextDraw();
    }
    else {
        m_doneMain = true;
        m_doneContext = true;
    }
}

bool DrawImageViewsSynchronizer::isZoomView() const {
    bool zoomView = false;
    if ( m_drawZoom ){
        zoomView = true;
    }
    return zoomView;
}

void DrawImageViewsSynchronizer::render( QList<std::shared_ptr<Layer> >& datas,
              const std::shared_ptr<RenderRequest>& request ){
    if (!m_doneMain || !m_doneContext || !m_doneZoom ){
        return;
    }

    //Store the data & request.
    m_datas = datas;
    m_request = request;

    if ( m_drawMain && request->isRequestMain()){

        //Decide which views need to be drawn.
        m_doneMain = false;
    }
    if ( m_drawContext && request->isRequestContext() ){
        m_doneContext = false;
    }
    if ( m_drawZoom && request->isRequestZoom()){
        m_doneZoom = false;
    }
    _startNextDraw();
}

void DrawImageViewsSynchronizer::setViewDraw( std::shared_ptr<DrawStackSynchronizer> stackDraw ){
    m_drawMain = stackDraw;
    connect ( m_drawMain.get(), SIGNAL( done(bool) ), this, SLOT(_doneMain(bool)));
}

void DrawImageViewsSynchronizer::setViewDrawContext( std::shared_ptr<DrawStackSynchronizer> stackDraw ){
    m_drawContext = stackDraw;
    connect( m_drawContext.get(), SIGNAL( done(bool)), this, SLOT( _doneContext(bool)));
}

void DrawImageViewsSynchronizer::setViewDrawZoom( std::shared_ptr<DrawStackSynchronizer> stackDraw ){
    m_drawZoom = stackDraw;
    connect( m_drawZoom.get(), SIGNAL( done(bool)), this, SLOT( _doneZoom(bool)));
}

void DrawImageViewsSynchronizer::_startNextDraw(){
    if ( !m_doneMain ){
        //Start the main renderer
        m_drawMain-> _render( m_datas, m_request);
    }
    else if ( !m_doneContext ){
        //Center the image and use default zoom for the context.
        std::shared_ptr<RenderRequest> contextRequest( new RenderRequest(*m_request.get()) );
        contextRequest->setZoom( DataSource::ZOOM_DEFAULT );
        contextRequest->setPan(  QPointF(nan(""), nan("")) );
        m_drawContext-> _render(m_datas, contextRequest);
    }
    else if ( !m_doneZoom ){
        m_drawZoom->_render(m_datas, m_request);
    }
}


DrawImageViewsSynchronizer::~DrawImageViewsSynchronizer(){

}
}
}
