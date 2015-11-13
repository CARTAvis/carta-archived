/**
 * These classes illustrate how to wrap the basic functionality of IRemoteVGView to create
 * higher-level features.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "IConnector.h"
#include "CartaLib/IRemoteVGView.h"
#include <QObject>

namespace Carta
{
namespace Hacks
{
class ManagedLayerViewInh;

/// derive from this class to create custom managed layers
class ManagedLayer : public QObject
{
    Q_OBJECT

public:

    typedef int ID;

    ManagedLayer( ManagedLayerViewInh *, QString name );

    virtual QString
    layerName() { return m_layerName; }

    /// reimplement this to react to view resizes
    virtual void
    onResize( const QSize & size ) { Q_UNUSED( size ); }

    /// call this to update the rendering of raster
    void setRaster( const QImage & image);

    /// this returns a unique ID of the layer (wrt. to the manager)
    ID layerID() { return m_id; }

    virtual
    ~ManagedLayer() { }

protected:

    ManagedLayerViewInh * m_mlv = nullptr;
    QString m_layerName;
    ID m_id = -1;
};

class ManagedLayerViewInh : public QObject
{
    Q_OBJECT

public:

    ManagedLayerViewInh( QString viewName, IConnector * connector, QObject * parent = nullptr );

//    template < class ILayer, typename ... Args >
//    std::shared_ptr < ILayer >
//    create( Args && ... args )
//    {
//        std::shared_ptr < ILayer > layer =
//            std::make_shared < ILayer > ( std::forward < Args > ( args ) ... );
//        return layer;
//    }

    virtual
    ~ManagedLayerViewInh() { }

    IConnector *
    connector();

private:

    virtual ManagedLayer::ID
    p_addLayer( ManagedLayer * layer );

    friend class ManagedLayer;

    std::vector < ManagedLayer * > m_layers;
    IConnector * m_connector = nullptr;
    ManagedLayer::ID m_nextId = 0;

    Carta::Lib::IRemoteVGView::UniquePtr m_rvgv = nullptr;
};

class EyesLayer : public ManagedLayer
{
    Q_OBJECT

public:

    EyesLayer( ManagedLayerViewInh * mlv, QString layerName )
        : ManagedLayer( mlv, layerName )
    { }

private:
};

class BouncyLayer : public ManagedLayer
{
    Q_OBJECT

public:

    BouncyLayer( ManagedLayerViewInh * mlv, QString layerName )
        : ManagedLayer( mlv, layerName ) { }
};

//std::shared_ptr < ManagedLayerViewInh >
//createManagedLayerView( IConnector * connector, QString viewName )
//{
//    return std::make_shared < ManagedLayerViewInh > ();
//}

/*
static int
apiTest()
{
    IConnector * connector = nullptr;

    // create a new view
    ManagedLayerViewInh * mlv = new ManagedLayerViewInh( "mlv1", connector, nullptr);

    // create a layer in this view
    auto layer1 = new EyesLayer( mlv );


    Q_UNUSED( layer1);
    return 0;
}

static auto foo = apiTest();

*/

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

//static int test = apiTest();
}
}
