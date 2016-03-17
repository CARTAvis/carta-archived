#include "DrawGroupSynchronizer.h"
#include "Data/Image/Layer.h"
#include "Data/Image/LayerCompositionModes.h"
#include "CartaLib/IRemoteVGView.h"

#include <QtCore/QDebug>

namespace Carta {

namespace Data {


DrawGroupSynchronizer::DrawGroupSynchronizer( ){
    m_repaintFrameQueued = false;
}


void DrawGroupSynchronizer::render(
        std::vector<int> frames, const Carta::Lib::KnownSkyCS& cs, int topIndex ){
    if ( m_repaintFrameQueued ){
        return;
    }
    m_repaintFrameQueued = true;
    int dataCount = m_layers.size();
    m_renderCount = 0;
    m_redrawCount = dataCount;
    int stackIndex = 0;
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_layers[i]->_isVisible() ){
            connect( m_layers[i].get(), SIGNAL(renderingDone()),
                    this, SLOT(_scheduleFrameRepaint()), Qt::UniqueConnection);
            bool topOfStack = false;
            if ( i == topIndex ){
                topOfStack = true;
            }
            m_layers[i]->_render( frames, cs, topOfStack );
            stackIndex++;
        }
    }
}

void DrawGroupSynchronizer::_scheduleFrameRepaint(){
    m_renderCount++;
    //If we are still waiting for other layers to finish, do nothing.
    if ( m_renderCount != m_redrawCount ) {
        return;
    }
    if ( m_imageSize.height() > 0 && m_imageSize.width() > 0 ){
        int dataCount = m_layers.size();
        if ( m_combineMode == LayerCompositionModes::PLUS ){
            std::shared_ptr<Carta::Lib::PixelMaskCombiner> pmc =
                    std::make_shared < Carta::Lib::PixelMaskCombiner > ();
            m_qImage = QImage(m_imageSize, QImage::Format_ARGB32 );
            for ( int i = 0; i < dataCount; i++ ){
                float alphaVal = m_layers[i]->_getMaskAlpha();
                pmc-> setAlpha( alphaVal );
                qint32 maskColor = m_layers[i]->_getMaskColor();
                pmc-> setMask( maskColor );
                QImage layerImage = m_layers[i]->_getQImage();
                pmc->combine( m_qImage, layerImage );
            }
        }
        else {
            if ( dataCount > 0 ){
                //Implement for now to just get one image.
                m_qImage = m_layers[0]->_getQImage();
            }
        }
    }
    m_repaintFrameQueued = false;
    emit done( m_qImage );
}

void DrawGroupSynchronizer::setCombineMode( const QString& mode ){
    m_combineMode = mode;
}

void DrawGroupSynchronizer::setLayers( QList< std::shared_ptr<Layer> > layers){
    m_layers = layers;
}

void DrawGroupSynchronizer::viewResize( const QSize& newSize ){
    m_imageSize = newSize;
}


DrawGroupSynchronizer::~DrawGroupSynchronizer(){

}

}
}
