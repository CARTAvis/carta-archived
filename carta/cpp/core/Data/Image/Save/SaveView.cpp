#include "SaveView.h"
#include <QDebug>

namespace Carta {
namespace Data {

SaveView::SaveView(){
    m_raster = QImage( 100, 100, QImage::Format_ARGB32_Premultiplied );
    m_raster.fill( 0xff000000 );
}

const QImage & SaveView::getBuffer(){
    return m_buffer;
}

qint64 SaveView::scheduleRepaint( qint64 id ){
    m_buffer = m_raster;
    if ( !m_buffer.isNull()){
        QPainter painter( & m_buffer);
        Carta::Lib::VectorGraphics::VGListQPainterRenderer renderer;
        renderer.render( m_vgList, painter);
        painter.end();
    }
    if ( id == - 1 ) {
        id = m_lastRepaintId++;
    }
    m_lastRepaintId = id;
    return id;
}

void SaveView::setRaster( const QImage & image ){
    m_raster = image;
}

void SaveView::setVectorGraphics( const Lib::IRemoteVGView::VGList & vglist ){
    m_vgList = vglist;
}


SaveView::~SaveView(){

}

}
}
