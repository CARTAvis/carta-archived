
#include "Data/Image/Render/RenderResponse.h"
#include <QDebug>

namespace Carta {

namespace Data {

RenderResponse::RenderResponse( const QImage image,
            const Carta::Lib::VectorGraphics::VGList vgList,
            //bool save,
            const QString& layerName ){
    m_image = image;
    m_graphics = vgList;
    m_layerId = layerName;
}

QImage RenderResponse::getImage() const {
    return m_image;
}

QString RenderResponse::getLayerName() const {
    return m_layerId;
}


Carta::Lib::VectorGraphics::VGList RenderResponse::getVectorGraphics() const {
    return m_graphics;
}


RenderResponse::~RenderResponse(){

}

}
}
