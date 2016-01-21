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
    m_rasterBuff = image;
    m_isRaster = true;
    m_mlv-> scheduleRepaint();
}

ManagedLayerView::ManagedLayerView( QString viewName,
                                    IConnector * connector,
                                    QObject * parent )
    : QObject( parent )
{
    m_connector = connector;

    // make the low level layered view
    m_lrv.reset( new Carta::Lib::LayeredRemoteVGView( m_connector, viewName, this ) );

    // listen for resize events
    connect( m_lrv.get(), & Carta::Lib::LayeredRemoteVGView::sizeChanged,
             this, & Me::clientSizeChangedCB );

    // listen for input events
    connect( m_lrv.get(), & Carta::Lib::LayeredRemoteVGView::inputEvent,
             this, & Me::inputEventCB );
}

void
ManagedLayerView::setInputLayers( const std::vector < ManagedLayerBase::ID > & list )
{
    for ( auto layer : m_layers ) {
        layer-> m_hasInput = std::find( list.begin(), list.end(), layer-> layerID() ) != list.end();
    }

    emit layersUpdated();
}

//IConnector *
//ManagedLayerView::connector()
//{
//    return m_connector;
//}

qint64
ManagedLayerView::scheduleRepaint()
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

    return m_lrv-> scheduleRepaint();
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
        }
    }
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
