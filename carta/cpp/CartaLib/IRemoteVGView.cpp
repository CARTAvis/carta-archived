/**
 *
 **/

#include "IRemoteVGView.h"
#include "core/IConnector.h"
#include "VectorGraphics/VGList.h"

#include <QTimer>
#include <QPainter>

namespace Carta
{
namespace Lib
{
LayeredRemoteVGView::SharedPtr
LayeredRemoteVGView::create( IConnector * connector, QString viewName, QObject * parent )
{
    //        return std::make_shared< LayeredRemoteVGView > ( connector, viewName, parent);
    // we love C++ /sarcasm
    LayeredRemoteVGView * lv = new LayeredRemoteVGView( connector, viewName, parent );
    return std::shared_ptr < LayeredRemoteVGView > ( lv );
}

int
LayeredRemoteVGView::nRasterLayers()
{
    return m_rasterLayers.size();
}

int
LayeredRemoteVGView::nVGLayers()
{
    return m_vgLayers.size();
}

int
LayeredRemoteVGView::nLayers()
{
    return nRasterLayers() + nVGLayers();
}

void
LayeredRemoteVGView::resetLayers()
{
    m_vgLayers.clear();
    m_rasterLayers.clear();
}

void
LayeredRemoteVGView::setRasterLayer( int layer, const QImage & img )
{
    CARTA_ASSERT( layer >= 0 && layer < 1000 );
    if ( int ( m_rasterLayers.size() ) <= layer ) {
        m_rasterLayers.resize( layer + 1 );
    }
    m_rasterLayers[layer].qimg = img;
}

void
LayeredRemoteVGView::setRasterLayerCombiner( int layer, IQImageCombiner::SharedPtr combiner )
{
    CARTA_ASSERT( layer >= 0 && layer < 1000 );
    if ( int ( m_rasterLayers.size() ) <= layer ) {
        m_rasterLayers.resize( layer + 1 );
    }
    m_rasterLayers[layer].combiner = combiner;
}

void
LayeredRemoteVGView::setVGLayer( int layer, const VectorGraphics::VGList & vglist )
{
    CARTA_ASSERT( layer >= 0 && layer < 1000 );
    if ( int ( m_vgLayers.size() ) <= layer ) {
        m_vgLayers.resize( layer + 1 );
    }
    m_vgLayers[layer].vglist = vglist;
}

QSize
LayeredRemoteVGView::getClientSize()
{
    return m_vgView-> getClientSize();
}

qint64
LayeredRemoteVGView::scheduleRepaint( qint64 id )
{
    if ( id == - 1 ) {
        id = m_repaintId++;
    }
    m_repaintId = id;
    if ( ! m_timer-> isActive() ) {
        m_timer-> start();
    }
    return m_repaintId;
}

LayeredRemoteVGView::LayeredRemoteVGView( IConnector * connector,
                                          QString viewName,
                                          QObject * parent )
    : QObject( parent )
{
    m_vgView.reset( connector-> makeRemoteVGView( viewName ));

    /// forward signals directly
    connect( m_vgView.get(), SIGNAL( sizeChanged() ), this, SIGNAL( sizeChanged() ) );
    connect( m_vgView.get(), SIGNAL( repainted() ), this, SIGNAL( repainted() ) );

    m_timer = new QTimer( this );
    m_timer-> setSingleShot( true );
    m_timer-> setInterval( 1 );
    connect( m_timer, & QTimer::timeout, this, & Me::timerCB );
}

void
LayeredRemoteVGView::timerCB()
{
    // figure out the size of the buffer (max of the raster sizes)
    QSize size( 1, 1);
    for ( auto & layer : m_rasterLayers ) {
        size = size.expandedTo( layer.qimg.size());
    }

    QImage buff( size, QImage::Format_ARGB32_Premultiplied );
    buff.fill( QColor( 0, 0, 0, 255 ) );

    // combine all raster layers into buff using their respective compositors
//    if ( m_rasterLayers.size() > 0 ) {
//        // the background will be the same size as the first layer, but filled with black
//        buff = m_rasterLayers[0].qimg;
//        buff.fill( QColor( 0, 0, 0, 255 ) );

        // now go through the layers and paint them on top of the last result
        for ( auto & layer : m_rasterLayers ) {
            IQImageCombiner::SharedPtr combiner = layer.combiner;
            if ( ! combiner ) {
                combiner = std::make_shared < DefaultCombiner > ();
            }
            combiner->combine( buff, layer.qimg );
        }

//        for ( size_t i = 0 ; i < m_rasterLayers.size() ; ++i ) {
//            IQImageCombiner::SharedPtr combiner = m_rasterLayers[i].combiner;
//            if ( ! combiner ) {
//                combiner = std::make_shared < DefaultCombiner > ();
//            }
//            combiner->combine( buff, m_rasterLayers[i].qimg );
//        }
//    }

    m_vgView-> setRaster( buff );

    // concatenate all VG lists into one
    VectorGraphics::VGComposer composer;
    for ( auto & vglayer : m_vgLayers ) {
        composer.append < VectorGraphics::Entries::Reset > ();
        composer.appendList( vglayer.vglist );
    }

    m_vgView-> setVG( composer.vgList());

    // now we render all VG layers on top of buff
//    for ( auto & vglayer : m_vgLayers ) {
//        VectorGraphics::VGListQPainterRenderer renderer;
//        QPainter painter( & buff );
//        renderer.render( vglayer.vglist, painter );
//    }

    // render all layers on top of each other
//    QColor color( qrand() % 255, qrand() % 255, qrand() % 255 );
//    QImage img( 100, 100, QImage::Format_ARGB32_Premultiplied);
//    img.fill( color);
//    m_vgView-> setRaster( buff );

    // schedule repaint
    (void) m_vgView-> scheduleRepaint( m_repaintId );
} // timerCB
}
}
