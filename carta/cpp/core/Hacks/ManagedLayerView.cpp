/**
 *
 **/

#include "ManagedLayerView.h"

namespace Carta
{
namespace Hacks
{
ManagedLayerBase::ManagedLayerBase( ManagedLayerView * mlv, QString name )
{
    m_mlv = mlv;
    m_layerName = name;
    m_id = m_mlv-> p_addLayer( this );
}

void
ManagedLayerBase::setRaster( const QImage & image )
{
    m_vgListBuff = Carta::Lib::VectorGraphics::VGList();
    m_rasterBuff = image;
    m_isRaster = true;
    m_mlv-> scheduleRepaint();
}

void
ManagedLayerBase::setVG( const Lib::VectorGraphics::VGList & vgList )
{
    m_rasterBuff = QImage();
    m_vgListBuff = vgList;
    m_isRaster = false;
    m_mlv-> scheduleRepaint();
}

ManagedLayerView::ManagedLayerView( QString viewName,
                                    IConnector * connector,
                                    QObject * parent )
    : QObject( parent )
{
    m_connector = connector;

    // make the low level layered view
//    m_lrv.reset( new Carta::Lib::LayeredRemoteVGView( m_connector, viewName, this ) );
    m_lrv.reset( new Carta::Lib::LayeredViewArbitrary( m_connector, viewName, this ) );

    // listen for resize events
    connect( m_lrv.get(), & Carta::Lib::LayeredViewArbitrary::sizeChanged,
             this, & Me::clientSizeChangedCB );

    // listen for input events
    connect( m_lrv.get(), & Carta::Lib::LayeredViewArbitrary::inputEvent,
             this, & Me::inputEventCB );

    // connect the repaint timer
    m_repaintTimer.setInterval( 0 );
    m_repaintTimer.setSingleShot( true );
    connect( & m_repaintTimer, & QTimer::timeout, this, & Me::repaintTimerCB );
}

void
ManagedLayerView::setInputLayers( const std::vector < ManagedLayerBase::ID > & list )
{
    for ( auto layer : m_layers ) {
        layer-> m_hasInput = std::find( list.begin(), list.end(), layer-> layerID() ) != list.end();
    }

    emit layersUpdated();
}

void
ManagedLayerView::moveLayersUp( const std::vector < ManagedLayerBase::ID > & list )
{
    // find the indices of the layers in m_layers
    std::vector < size_t > indices;
    for ( auto & id : list ) {
        bool found = false;
        for ( size_t ind = 0 ; ind < m_layers.size() ; ind++ ) {
            ManagedLayerBase * layer = m_layers[ind];
            if ( layer-> layerID() == id ) {
                found = true;
                indices.push_back( ind );
                break;
            }
        }
        if ( ! found ) {
            qCritical() << "Could not move up layer" << id << "because it was not found";
        }
    }

    // sort the indices
    std::sort( indices.begin(), indices.end() );

    // remove layers that are already in place
    size_t ind = 0;
    while ( indices.size() > 0 && indices[0] == ind ) {
        indices.erase( indices.begin() );
        ind++;
    }

    if ( indices.size() == 0 ) {
        // nothing to do
        return;
    }

    // move the remaining layers one at a time
    qDebug() << "Moving up layers" << indices;
    for ( auto ind : indices ) {
        std::swap( m_layers[ind - 1], m_layers[ind] );
    }

    scheduleRepaint();
    emit layersUpdated();
} // moveLayersUp

void
ManagedLayerView::moveLayersDown( const std::vector < ManagedLayerBase::ID > & list )
{
    // find the indices of the layers in m_layers
    std::vector < size_t > indices;
    for ( auto & id : list ) {
        bool found = false;
        for ( size_t ind = 0 ; ind < m_layers.size() ; ind++ ) {
            ManagedLayerBase * layer = m_layers[ind];
            if ( layer-> layerID() == id ) {
                found = true;
                indices.push_back( ind );
                break;
            }
        }
        if ( ! found ) {
            qCritical() << "Could not move down layer" << id << "because it was not found";
        }
    }

    // sort the indices
    std::sort( indices.begin(), indices.end(), std::greater < size_t > () );

    // remove layers that are already in place
    size_t ind = m_layers.size() - 1;
    while ( indices.size() > 0 && indices[0] == ind ) {
        indices.erase( indices.begin() );
        ind--;
    }

    if ( indices.size() == 0 ) {
        // nothing to do
        return;
    }

    // move the remaining layers one at a time
    qDebug() << "Moving down layers" << indices;
    for ( auto ind : indices ) {
        std::swap( m_layers[ind + 1], m_layers[ind] );
    }

    scheduleRepaint();
    emit layersUpdated();
} // moveLayersDown

void
ManagedLayerView::removeLayers( const std::vector < ManagedLayerBase::ID > & list )
{
    auto oldLayers = m_layers;
    m_layers.clear();
    for ( auto layer : oldLayers ) {
        if ( std::find( list.begin(), list.end(), layer-> layerID() ) != list.end() ) {
            layer-> onLayerRemoved();
        }
        else {
            m_layers.push_back( layer );
        }
    }
    if ( oldLayers.size() == m_layers.size() ) {
        return;
    }
    scheduleRepaint();
    emit layersUpdated();
} // removeLayers

//IConnector *
//ManagedLayerView::connector()
//{
//    return m_connector;
//}

qint64
ManagedLayerView::scheduleRepaint()
{
    if ( ! m_repaintTimer.isActive() ) {
        m_repaintTimer.start();
    }
    m_repaintId++;
    return m_repaintId;
} // scheduleRepaint

void
ManagedLayerView::clientSizeChangedCB()
{
    // tell all layers about the size changes
    auto clientSize = m_lrv-> getClientSize();
    for ( auto * layer : m_layers ) {
        layer-> onResize( clientSize );
    }
}

void
ManagedLayerView::inputEventCB( Lib::InputEvent e )
{
    qDebug() << "Received input event" << e.json()["type"].toString();

    // now go through our list of input layers and invoke their input event callbacks
    for ( auto layer : m_layers ) {
        if ( layer-> hasInput() ) {
            layer-> onInputEvent( e );

            // abort once the event is consumed
            if ( e.isConsumed() ) {
                break;
            }
        }
    }

//    if( CARTA_RUNTIME_CHECKS && ! e.isConsumed()) {
//        qWarning() << "Unconsumed event" << e.type();
//    }
} // inputEventCB

/*
void
ManagedLayerView::repaintTimerCB()
{
    m_lrv-> resetLayers();
    int rasterInd = 0;
    int vgInd = 0;
    for ( auto * l : m_layers ) {
        if ( ! l-> isRaster() ) {
            continue;
        }
        m_lrv-> setRasterLayer( rasterInd, l-> raster() );
        m_lrv-> setRasterLayerCombiner( rasterInd, l-> rasterCombiner() );
        rasterInd++;
    }
    for ( auto * l : m_layers ) {
        if ( l-> isRaster() ) {
            continue;
        }
        m_lrv-> setVGLayer( vgInd, l-> vgList() );
        vgInd++;
    }

    m_lrv-> scheduleRepaint( m_repaintId );
} // inputEventCB
*/
void
ManagedLayerView::repaintTimerCB()
{
    m_lrv-> removeAllLayers();
    int layerInd = 0;

//    int vgInd = 0;
    for ( auto * l : m_layers ) {
        if ( l-> isRaster() ) {
            m_lrv-> setLayerRaster( layerInd, l-> raster() );
            m_lrv-> setLayerCombiner( layerInd, l-> rasterCombiner() );
        }
        else {
            m_lrv-> setLayerVG( layerInd, l-> vgList() );
        }
        layerInd++;
    }

//    for ( auto * l : m_layers ) {
//        if ( l-> isRaster() ) {
//            continue;
//        }
//        m_lrv-> setVGLayer( vgInd, l-> vgList() );
//        vgInd++;
//    }

    m_lrv-> scheduleRepaint( m_repaintId );
} // inputEventCB

ManagedLayerBase::ID
ManagedLayerView::p_addLayer( ManagedLayerBase * layer )
{
    m_layers.push_back( layer );
    auto currId = m_nextId;
    m_nextId++;
    return currId;
}

/*
static int
apiTest()
{
    // create a new managed layer view
    IConnector * connector = nullptr;
    auto mlv = new ManagedLayerViewInh( "mlv1", connector, nullptr );

    // add some layers to the view
    auto eyesLayer1 = new EyesLayer( mlv, "eyes1" );
    auto eyesLayer2 = new EyesLayer( mlv, "eyes two" );
    auto bouncyBallLayer1 = new BouncyLayer( mlv, "Bouncy" );

    return 7;
}
*/

//static int test = apiTest();
}
}
