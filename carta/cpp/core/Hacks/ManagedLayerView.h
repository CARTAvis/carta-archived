/**
 * These classes illustrate how to wrap the basic functionality of IRemoteVGView to create
 * higher-level features.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IRemoteVGView.h"
#include "core/IConnector.h"
#include <QObject>
#include <QTimer>

namespace Carta
{
namespace Hacks
{
class ManagedLayerView;

/// derive from this class to create custom layers
class ManagedLayerBase : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( ManagedLayerBase );

public:

    typedef int ID;

    ///
    /// constructor - creates a link between the managed layered view and this layer
    ///
    ManagedLayerBase( ManagedLayerView * mlv, QString name );

    virtual QString
    layerName() { return m_layerName; }

    /// reimplement this to react to layer removed, the default is to auto-destruct
    virtual void
    onLayerRemoved()
    {
        this->deleteLater();
    }

    /// reimplement this to react to view resizes
    virtual void
    onResize( const QSize & size ) { Q_UNUSED( size ); }

    /// reimplement this to react to input events
    /// consume the event if you don't want it to propagate further
    virtual void
    onInputEvent( Carta::Lib::InputEvent & event ) { Q_UNUSED( event ); }

    /// call this to update the rendering of raster
    void
    setRaster( const QImage & image );

    /// call this to update the rendering vector graphics
    void
    setVG( const Carta::Lib::VectorGraphics::VGList & vgList );

    /// does the layer have input?
    bool
    hasInput()
    {
        return m_hasInput;
    }

    /// returns a unique ID of the layer (wrt. to the manager)
    ID
    layerID() { return m_id; }

    virtual
    ~ManagedLayerBase() { }

private:

    /// pointer to the managed layer view
    ManagedLayerView * m_mlv = nullptr;

    /// user assigned layer name
    QString m_layerName;

    /// unique id assigned to us by managed layer view
    ID m_id = - 1;

    // apis used be the manager to keep extra info on layers
    bool
    isRaster() { return m_isRaster; }

    bool m_isRaster = true;
    QImage m_rasterBuff;
    Carta::Lib::IQImageCombiner::SharedPtr m_rasterCombiner;
    Carta::Lib::VectorGraphics::VGList m_vgListBuff;

    Carta::Lib::IQImageCombiner::SharedPtr
    rasterCombiner() { return m_rasterCombiner; }

    const QImage &
    raster() { return m_rasterBuff; }

    const Carta::Lib::VectorGraphics::VGList &
    vgList() { return m_vgListBuff; }

    /// does the layer receive input?
    bool m_hasInput = false;

    friend class ManagedLayerView;
};

class ManagedLayerView : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( ManagedLayerView );

public:

    ManagedLayerView( QString viewName, IConnector * connector, QObject * parent = nullptr );

//    template < class ILayer, typename ... Args >
//    std::shared_ptr < ILayer >
//    create( Args && ... args )
//    {
//        std::shared_ptr < ILayer > layer =
//            std::make_shared < ILayer > ( std::forward < Args > ( args ) ... );
//        return layer;
//    }

    /// get the list of layers
    const std::vector < ManagedLayerBase * > &
    layers() const { return m_layers; }

    /// set which layers are to receive input events
    void
    setInputLayers( const std::vector < ManagedLayerBase::ID > & list = { } );

    /// move the specified layers 'up'
    void
    moveLayersUp( const std::vector < ManagedLayerBase::ID > & list );

    void
    moveLayersDown( const std::vector < ManagedLayerBase::ID > & list );

    void
    removeLayers( const std::vector < ManagedLayerBase::ID > & list );

    virtual
    ~ManagedLayerView() { }

    QString
    viewName() { return m_lrv-> viewName(); }

//    IConnector *
//    connector();

signals:

    /// emitted when the list of layers has been modified (e.g. by user interface)
    void
    layersUpdated();

public slots:

    /// schedules a repaint and returns a repaint ID
    qint64
    scheduleRepaint();

private slots:

    // callback for client size changes from LayeredRemoteVGView
    void
    clientSizeChangedCB();

    // callback for input events
    void
    inputEventCB( Carta::Lib::InputEvent e );

    // internal repaint timer callback
    void
    repaintTimerCB();

private:

    virtual ManagedLayerBase::ID
    p_addLayer( ManagedLayerBase * layer );

    friend class ManagedLayerBase;

    IConnector * m_connector = nullptr;
    ManagedLayerBase::ID m_nextId = 0;

    // we use layered remote view to do the rendering
//    Carta::Lib::LayeredRemoteVGView::SharedPtr m_lrv = nullptr;
    Carta::Lib::LayeredViewArbitrary::SharedPtr m_lrv = nullptr;

    // here we keep track of all our layers, and since we don't own the layers
    // we keep raw pointers
    std::vector < ManagedLayerBase * > m_layers;

    // repaint timer - to allow multiple repaint requests, but only repaint one
    QTimer m_repaintTimer;

    // repaint id
    qint64 m_repaintId = 0;
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
