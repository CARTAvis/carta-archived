#include "DrawStackSynchronizer.h"
#include "Data/Image/Layer.h"
#include "Data/Image/LayerCompositionModes.h"
#include "CartaLib/IRemoteVGView.h"


#include <QtCore/QDebug>

namespace Carta {

namespace Data {


DrawStackSynchronizer::DrawStackSynchronizer( Carta::Lib::LayeredRemoteVGView* view ){
    m_repaintFrameQueued = false;
    m_selectIndex = -1;
    m_view.reset( view );
    connect( m_view.get(), SIGNAL(sizeChanged()), this, SIGNAL( viewResize() ) );
}

QSize DrawStackSynchronizer::getClientSize() const {
    return m_view->getClientSize();
}


void DrawStackSynchronizer::_repaintFrameNow(){
    m_view->scheduleRepaint();
    m_repaintFrameQueued = false;
}


void DrawStackSynchronizer::_render( QList<std::shared_ptr<Layer> >& datas,
        std::vector<int> frames, const Carta::Lib::KnownSkyCS& cs, int topIndex ){
    if ( m_repaintFrameQueued ){
        return;
    }
    int dataCount = datas.size();
    m_layers = datas;
    m_selectIndex = topIndex;
    m_renderCount = 0;
    m_redrawCount = dataCount;
    for ( int i = 0; i < dataCount; i++ ){
        if ( datas[i]->_isVisible() ){
            connect( datas[i].get(), SIGNAL(renderingDone()),
                    this, SLOT(_scheduleFrameRepaint()), Qt::UniqueConnection);
            bool topOfStack = false;
            if ( i == topIndex ){
                topOfStack = true;
            }
            datas[i]->_render( frames, cs, topOfStack );
        }
    }
    if ( dataCount == 0 ){
        m_view->resetLayers();
        m_repaintFrameQueued = true;
        QMetaObject::invokeMethod( this, "_repaintFrameNow", Qt::QueuedConnection );
    }
}

void DrawStackSynchronizer::_scheduleFrameRepaint(){
    // if reload is already pending, do nothing
    m_renderCount++;
    if ( m_repaintFrameQueued || (m_renderCount != m_redrawCount ) ) {
        return;
    }

    m_view->resetLayers();

    //We want the selected index to be the last one in the stack.
    int dataCount = m_layers.size();
    int stackIndex = 0;
    for ( int i = 0; i < dataCount; i++ ){
        int dIndex = ( m_selectIndex + i + 1) % dataCount;
        m_layers[dIndex]->disconnect( this );
        bool layerEmpty = m_layers[dIndex]->_isEmpty();
        bool layerVisible = m_layers[dIndex]->_isVisible();
        if ( layerVisible && !layerEmpty){
            QImage image = m_layers[dIndex]->_getQImage();
            Carta::Lib::VectorGraphics::VGList graphicsList = m_layers[dIndex]->_getVectorGraphics();
            m_view->setRasterLayer( stackIndex, image );
            std::shared_ptr<Carta::Lib::AlphaCombiner> alphaCombine =
                    std::make_shared<Carta::Lib::AlphaCombiner>();
            float alphaVal = m_layers[dIndex]->_getMaskAlpha();
            alphaCombine-> setAlpha( alphaVal );
            m_view->setRasterLayerCombiner( stackIndex, alphaCombine );
            m_view->setVGLayer( stackIndex, graphicsList );
            stackIndex++;
        }
    }
    m_repaintFrameQueued = true;
    QMetaObject::invokeMethod( this, "_repaintFrameNow", Qt::QueuedConnection );
}


DrawStackSynchronizer::~DrawStackSynchronizer(){

}

}
}
