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
//LayeredRemoteVGView::SharedPtr
//LayeredRemoteVGView::create( IConnector * connector, QString viewName, QObject * parent )
//{
//    //        return std::make_shared< LayeredRemoteVGView > ( connector, viewName, parent);
//    // we love C++ /sarcasm
//    LayeredRemoteVGView * lv = new LayeredRemoteVGView( connector, viewName, parent );
//    return std::shared_ptr < LayeredRemoteVGView > ( lv );
//}

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

QString
LayeredRemoteVGView::viewName()
{
    return m_vgView-> getRVGViewName();
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
        id = m_repaintId + 1;
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
    m_vgView.reset( connector-> makeRemoteVGView( viewName ) );

    // forward repainted signals directly
    connect( m_vgView.get(), SIGNAL( repainted( qint64 ) ), this, SIGNAL( repainted( qint64 ) ) );

    // forward input signals directly
    connect( m_vgView.get(), SIGNAL( inputEvent( InputEvent ) ), this,
             SIGNAL( inputEvent( InputEvent ) ) );

    //    connect( m_vgView.get(), SIGNAL( sizeChanged() ), this, SIGNAL( sizeChanged() ) );
    connect( m_vgView.get(), & IRemoteVGView::sizeChanged, this, & Me::p_sizeChangedCB );

    m_timer = new QTimer( this );
    m_timer-> setSingleShot( true );
    m_timer-> setInterval( 1 );
    connect( m_timer, & QTimer::timeout, this, & Me::p_timerCB );
}

void
LayeredRemoteVGView::p_timerCB()
{
    QSize size = getClientSize();

    QImage buff( size, QImage::Format_ARGB32_Premultiplied );
    buff.fill( QColor( 0, 0, 0, 0 ) );

    // now go through the layers and paint them on top of the last result
    for ( auto & layer : m_rasterLayers ) {
        IQImageCombiner::SharedPtr combiner = layer.combiner;
        if ( ! combiner ) {
            combiner = std::make_shared < DefaultCombiner > ();
        }
        combiner->combine( buff, layer.qimg );
    }
    m_vgView-> setRaster( buff );

    // concatenate all VG lists into one
    VectorGraphics::VGComposer composer;
    for ( auto & vglayer : m_vgLayers ) {
        composer.append < VectorGraphics::Entries::Reset > ();
        composer.appendList( vglayer.vglist );
    }

    // render the combined vector graphics list
    m_vgView-> setVG( composer.vgList() );

    // schedule repaint
    (void) m_vgView-> scheduleRepaint( m_repaintId );
} // p_timerCB

void
LayeredRemoteVGView::p_sizeChangedCB()
{
    emit sizeChanged();
}

LayeredViewArbitrary::LayeredViewArbitrary( IConnector * connector,
                                            QString viewName,
                                            QObject * parent )
    : QObject( parent )
{
    // create the basic IRemoteVGView
    m_vgView.reset( connector-> makeRemoteVGView( viewName ) );

    // forward repainted signals directly
    connect( m_vgView.get(), SIGNAL( repainted( qint64 ) ), this, SIGNAL( repainted( qint64 ) ) );

    // forward input signals directly
    connect( m_vgView.get(), SIGNAL( inputEvent( InputEvent ) ), this,
             SIGNAL( inputEvent( InputEvent ) ) );

    // listen to client size changes
    connect( m_vgView.get(), & IRemoteVGView::sizeChanged, this, & Me::sizeChanged );

//    connect( m_vgView.get(), & IRemoteVGView::sizeChanged, this, & Me::p_sizeChangedCB );

    // setup a timer for redrawing (this allows the user to call scheduleRepaint() multiple
    // times, but only one redraw actually happens
    m_timer = new QTimer( this );
    m_timer-> setSingleShot( true );
    m_timer-> setInterval( 1 );
    connect( m_timer, & QTimer::timeout, this, & Me::p_timerCB );
}

size_t
LayeredViewArbitrary::nLayers() const
{
    return m_layers.size();
}

void
LayeredViewArbitrary::removeAllLayers()
{
    m_layers.resize( 0 );
}

void
LayeredViewArbitrary::setLayerRaster( int ind, const QImage & img, bool hasTransparentPixels )
{
    auto & l = linfo( ind );
    l.qimg = img;
    l.hasTransparentPixels = hasTransparentPixels && img.hasAlphaChannel();
    l.type = LayerType::Raster;
    l.vglist = VectorGraphics::VGList();
}

void
LayeredViewArbitrary::setLayerRaster( int layer, const QImage & img )
{
    setLayerRaster( layer, img, img.hasAlphaChannel() );
}

LayeredViewArbitrary::LayerInfo &
LayeredViewArbitrary::linfo( int layer )
{
    CARTA_ASSERT( layer >= 0 && layer < 1000 );
    if ( int ( m_layers.size() ) <= layer ) {
        m_layers.resize( layer + 1 );
    }
    return m_layers[layer];
}

void
LayeredViewArbitrary::setLayerCombiner( int layer, IQImageCombiner::SharedPtr combiner )
{
    auto & l = linfo( layer );
    l.comb = combiner;

    if ( CARTA_RUNTIME_CHECKS ) {
        if ( l.type != LayerType::Raster ) {
            qCritical() << "Setting layer combiner for non-raster layer" << layer;
        }
    }
}

void
LayeredViewArbitrary::setLayerVG( int layer, const VectorGraphics::VGList & vglist )
{
    auto & l = linfo( layer );
    l.comb = nullptr;
    l.vglist = vglist;
    l.qimg = QImage();
    l.type = LayerType::VG;
}

void
LayeredViewArbitrary::setVGrenderedOnServer( bool flag )
{
    m_vgView-> setVGrenderedOnServer( flag );
}

bool
LayeredViewArbitrary::isVGrenderedOnServer()
{
    return m_vgView-> isVGrenderedOnServer();
}

QString
LayeredViewArbitrary::viewName()
{
    return m_vgView-> getRVGViewName();
}

QSize
LayeredViewArbitrary::getClientSize()
{
    return m_vgView-> getClientSize();
}

qint64
LayeredViewArbitrary::scheduleRepaint( qint64 id )
{
    if ( id == - 1 ) {
        id = m_repaintId + 1;
    }
    m_repaintId = id;
    if ( ! m_timer-> isActive() ) {
        m_timer-> start();
    }
    return m_repaintId;
}

void
LayeredViewArbitrary::p_timerCB()
{
    // figure out the size of the buffer (max of the raster sizes)
//    QSize size( 1, 1 );
//    for ( auto & layerInfo : m_layers ) {
//        if ( layerInfo.type != LayerType::Raster ) {
//            continue;
//        }
//        size = size.expandedTo( layerInfo.qimg.size() );
//    }

    QSize size = getClientSize();

    // figure out which layer (from top to bottom) is fully opaque
    // we do this because we don't need to render any layer below it
    int startLayer = 0;

    /// \todo implement this
    startLayer = m_layers.size();
    while ( 1 ) {
        startLayer--;
        if ( startLayer <= 0 ) {
            break;
        }

        // non-raster layer are not candidates
        if ( m_layers[startLayer].type != LayerType::Raster ) {
            continue;
        }

        // skip over invisible layers
        if ( ! m_layers[startLayer].visible ) {
            continue;
        }

        // skip rasters with possible transparent pixels
        if ( m_layers[startLayer].hasTransparentPixels ) {
            continue;
        }

        // skip layers that don't completely fill the size
        if ( m_layers[startLayer].qimg.size().width() < size.width() ||
             m_layers[startLayer].qimg.size().height() < size.height() ) {
            continue;
        }

        // at this point we found a layer that is:
        // - raster
        // - visible
        // - has no alpha pixels
        // - occupies every pixel
        // so we declare victory
        break;
    }

    // figure out the last layer we'll need to render on the server (basically
    // all the consecutive VG layers from the top can be delegated to the VGView)
    /// \todo implement this

    // prepare a buffer filled with black pixels
    QImage buff( size, QImage::Format_ARGB32_Premultiplied );
    buff.fill( QColor( 0, 0, 0, 255 ) );

    IQImageCombiner::SharedPtr defaultComb = std::make_shared < DefaultCombiner > ();

    // render all the layers from bottom to top
//    for ( auto & layerInfo : m_layers ) {
    for ( size_t i = startLayer ; i < m_layers.size() ; ++i ) {
        LayerInfo & layerInfo = m_layers[i];

        // skip invisible layers
        if ( ! layerInfo.visible ) {
            continue;
        }

        // for raster layers we use the image combiner to draw them
        if ( layerInfo.type == LayerType::Raster ) {
            if ( layerInfo.comb ) {
                layerInfo.comb-> combine( buff, layerInfo.qimg );
            }
            else {
                defaultComb-> combine( buff, layerInfo.qimg );
            }
        }

        // for VG layers we VGListQPainterRenderer
        else {
            QPainter painter( & buff );
            Carta::Lib::VectorGraphics::VGListQPainterRenderer renderer;
            renderer.render( layerInfo.vglist, painter );
            painter.end();
        }
    }

    m_vgView-> setRaster( buff );

    // schedule repaint
    (void) m_vgView-> scheduleRepaint( m_repaintId );
} // p_timerCB

// timerCB
}
}
