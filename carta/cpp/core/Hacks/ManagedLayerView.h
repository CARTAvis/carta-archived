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

class ManagedLayer : public QObject
{
    Q_OBJECT

public:

    ManagedLayer( ManagedLayerViewInh * );
    virtual
    ~ManagedLayer() { }

private:

    ManagedLayerViewInh * m_mlv = nullptr;

};

class ManagedLayerViewInh : public QObject
{
    Q_OBJECT

public:

    ManagedLayerViewInh( QString viewName, IConnector * connector, QObject * parent = nullptr);


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

    IConnector * connector();

private:

    virtual void p_addLayer( ManagedLayer * layer);

    friend class ManagedLayer;

    std::vector< ManagedLayer * > m_layers;
    IConnector * m_connector = nullptr;

    Carta::Lib::IRemoteVGView::UniquePtr m_rvgv = nullptr;

};

class EyesLayer : public ManagedLayer
{
    Q_OBJECT
public:

    EyesLayer( ManagedLayerViewInh * mlv );
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

}
}
