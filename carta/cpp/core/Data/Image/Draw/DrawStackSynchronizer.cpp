#include "DrawStackSynchronizer.h"
#include "Data/Image/Layer.h"
#include "Data/Image/LayerCompositionModes.h"
#include "CartaLib/IRemoteVGView.h"


#include <QtCore/QDebug>

namespace Carta {

namespace Data {


DrawStackSynchronizer::DrawStackSynchronizer( Carta::Lib::LayeredViewArbitrary* view ){
    m_repaintFrameQueued = false;
    m_selectIndex = -1;
    m_view.reset( view );
    // listen for resize events
    connect( m_view.get(), SIGNAL(sizeChanged()), this, SIGNAL( viewResize() ) );
    // listen for input events
    connect( m_view.get(), SIGNAL(inputEvent(InputEvent)),
    		this, SIGNAL( inputEvent(InputEvent)) );
}

void DrawStackSynchronizer::_clear(){
    m_view->removeAllLayers();
    QMetaObject::invokeMethod( this, "_repaintFrameNow", Qt::QueuedConnection );
}

QSize DrawStackSynchronizer::getClientSize() const {
    return m_view->getClientSize();
}


QList<std::shared_ptr<Layer> > DrawStackSynchronizer::_getLoadableData( const std::shared_ptr<RenderRequest>& request ){
	QList<std::shared_ptr<Layer> > loadables;
	QList<std::shared_ptr<Layer> > datas = request->getData();
	std::vector<int> frames = request->getFrames();
	int dataCount = datas.size();
	for ( int i = 0; i < dataCount; i++ ){
		if ( datas[i]->_isLoadable(frames) ){
			loadables.push_back( datas[i] );
		}
	}
	return loadables;
}

void DrawStackSynchronizer::_repaintFrameNow(){
    m_view->scheduleRepaint();

}


void DrawStackSynchronizer::_render( const std::shared_ptr<RenderRequest>& request ){
    if ( m_repaintFrameQueued ){
        emit done( false );
        return;
    }
    QSize clientSize = getClientSize();
    if ( clientSize.width() <= 1 || clientSize.height() <= 1 ){
        emit done( false );
        return;
    }
    m_repaintFrameQueued = true;
    QList<std::shared_ptr<Layer> > datas = _getLoadableData( request );
    int dataCount = datas.size();
    m_images.clear();
    m_layers = datas;
    int topIndex = request->getTopIndex();
    m_selectIndex = topIndex;
    m_renderCount = 0;
    m_redrawCount = dataCount;
    for ( int i = 0; i < dataCount; i++ ){
        if ( datas[i]->_isVisible() ){
            connect( datas[i].get(), SIGNAL(renderingDone(const std::shared_ptr<RenderResponse>&)),
                    this, SLOT(_scheduleFrameRepaint(const std::shared_ptr<RenderResponse>&)),
                    Qt::UniqueConnection);
            bool topOfStack = false;
            if ( i == topIndex ){
                topOfStack = true;
            }
            std::shared_ptr<RenderRequest> layerRequest( new RenderRequest(
                                   *request ));
            layerRequest->setStackTop( topOfStack );
            datas[i]->_render( layerRequest );
        }
    }
    if ( dataCount == 0 ){

        m_repaintFrameQueued = false;
        _clear();
        emit done( true );
    }
}


void DrawStackSynchronizer::_scheduleFrameRepaint( const std::shared_ptr<RenderResponse>& response ){

    if ( !m_repaintFrameQueued ){
        return;
    }
    // if reload is already pending, do nothing
    m_renderCount++;
    m_images[response->getLayerName()] = response;
    if ( m_renderCount != m_redrawCount ) {
        return;
    }
    m_view->removeAllLayers();

    //We want the selected index to be the last one in the stack.
    int dataCount = m_layers.size();
    int stackIndex = 0;
    for ( int i = 0; i < dataCount; i++ ){
        int dIndex = ( m_selectIndex + i + 1) % dataCount;
        m_layers[dIndex]->disconnect( this );
        bool layerEmpty = m_layers[dIndex]->_isEmpty();
        bool layerVisible = m_layers[dIndex]->_isVisible();
        if ( layerVisible && !layerEmpty){
            QString layerName = m_layers[dIndex]->_getLayerId();
            if ( m_images.contains( layerName ) ){
                QImage image = m_images[layerName]->getImage();
                Carta::Lib::VectorGraphics::VGList graphicsList = m_images[layerName]->getVectorGraphics();
                m_view->setLayerRaster( stackIndex, image, false );
                std::shared_ptr<Carta::Lib::AlphaCombiner> alphaCombine =
                        std::make_shared<Carta::Lib::AlphaCombiner>();
                float alphaVal = m_layers[dIndex]->_getMaskAlpha();
                alphaCombine-> setAlpha( alphaVal );
                m_view->setLayerCombiner( stackIndex, alphaCombine );
                stackIndex++;
                m_view->setLayerVG( stackIndex, graphicsList );
                stackIndex++;
            }
        }
    }

    emit done( true );
    QMetaObject::invokeMethod( this, "_repaintFrameNow", Qt::QueuedConnection );
    for ( int i = 0; i < dataCount; i++ ){
        m_layers[i]->_renderDone();
    }
    m_repaintFrameQueued = false;
}




DrawStackSynchronizer::~DrawStackSynchronizer(){

}

}
}
