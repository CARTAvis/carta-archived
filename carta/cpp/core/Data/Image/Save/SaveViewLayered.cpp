
#include "SaveViewLayered.h"
#include "Data/Image/Save/SaveView.h"
#include <QDebug>

namespace Carta {
namespace Data {

SaveViewLayered::SaveViewLayered():
      m_vgView( new SaveView()){
}

QImage SaveViewLayered::getImage() const {
    return m_vgView->getBuffer();
}


void SaveViewLayered::resetLayers(){
    m_vgLayers.clear();
    m_rasterLayers.clear();
}

void SaveViewLayered::setRasterLayer( int layer, const QImage & img ){
    CARTA_ASSERT( layer >= 0 && layer < 1000 );
    if ( int ( m_rasterLayers.size() ) <= layer ) {
        m_rasterLayers.resize( layer + 1 );
    }
    m_rasterLayers[layer].qimg = img;
}


void SaveViewLayered::setVectorGraphicsLayer( int layer, const Carta::Lib::VectorGraphics::VGList & vglist ){
    CARTA_ASSERT( layer >= 0 && layer < 1000 );
    if ( int ( m_vgLayers.size() ) <= layer ) {
        m_vgLayers.resize( layer + 1 );
    }
    m_vgLayers[layer].vglist = vglist;
}


void SaveViewLayered::paintLayers(){
    // figure out the size of the buffer (max of the raster sizes)
    QSize size( 1, 1 );
    for ( auto & layer : m_rasterLayers ) {
        size = size.expandedTo( layer.qimg.size() );
    }

    QImage buff( size, QImage::Format_ARGB32_Premultiplied );
    buff.fill( QColor( 0, 0, 0, 255 ) );

    // now go through the layers and paint them on top of the last result
    for ( auto & layer : m_rasterLayers ) {
        Carta::Lib::IQImageCombiner::SharedPtr combiner = layer.combiner;
        if ( ! combiner ) {
            combiner = std::make_shared < Carta::Lib::DefaultCombiner > ();
        }
        combiner->combine( buff, layer.qimg );
    }

    m_vgView-> setRaster( buff );

    // concatenate all VG lists into one
    Carta::Lib::VectorGraphics::VGComposer composer;
    for ( auto & vglayer : m_vgLayers ) {
        composer.append < Carta::Lib::VectorGraphics::Entries::Reset > ();
        composer.appendList( vglayer.vglist );
    }

    // render the combined vector graphics list
    m_vgView-> setVectorGraphics( composer.vgList() );

    // schedule repaint
    m_vgView-> scheduleRepaint( );
}


SaveViewLayered::~SaveViewLayered(){

}


}
}
