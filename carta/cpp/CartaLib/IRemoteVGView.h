#pragma once

#include "CartaLib.h"
#include "core/IView.h"
#include "VectorGraphics/VGList.h"
#include "InputEvents.h"

#include <QObject>
#include <QString>
#include <QImage>
//#include <QJsonObject>
//#include <QJsonDocument>

namespace Carta
{
namespace Lib
{

typedef InputEvents::JsonEvent InputEvent;

///
/// \brief An API specification for rendering graphical views to be displayed by clients.
///
/// It offers the basic building block of rendering raster and/or vector graphics
/// on the client. The vector graphics is always layed over the raster graphics.
///
/// The reasons for having an abstracted API for this:
/// a) desktop might want to implement this differently from server
/// b) one day someone might want to write a plugin...
///
class IRemoteVGView
    : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( IRemoteVGView );

public:

    typedef Carta::Lib::VectorGraphics::VGList VGList;

    /// constructor
    IRemoteVGView( QObject * parent ) : QObject( parent ) { }

    /// returns size of the view in client (the master's UI)
    virtual const QSize &
    getClientSize() = 0;

    /// returns the unique name of this view
    virtual const QString &
    getRVGViewName() = 0;

    /// sets the raster to be rendered in the view, as soon as possible,
    /// without synchronizing with VG
    virtual void
    setRaster( const QImage & image ) = 0;

    /// sets the VG to be rendered on top of the raster, as soon as possible
    /// without synchronizing with raster
    virtual void
    setVG( const VGList & vglist ) = 0;

    /// sets the raster and VG simultaneously, making sure both appear at the same time
    virtual void
    setRasterAndVG( const QImage & image, const VGList & vglist ) = 0;

    /// sets where VG rendered (client vs server)
    virtual void
    setVGrenderedOnServer( bool flag ) = 0;

    /// returns true if VG is rendered on server, false if on client
    virtual bool
    isVGrenderedOnServer() = 0;

    /// tell UI which events we want to receive
//    virtual void
//    enableInputEvent( InputEvent::Type type, QString name = QString() ) = 0;

public slots:

    /// schedule a refresh with a given ID
    /// this allows the caller to be notified when the latest painting has reached the client
    virtual qint64
    scheduleRepaint( qint64 id = - 1 ) = 0;

signals:

    /// this signal is emitted when the master UI changes the size of the view
    void
    sizeChanged();

    /// emitted when client repainted the view
    void
    repainted( qint64 id );

    /// emitted when client sends us a gesture
    void
    inputEvent( InputEvent e );
};

/// API specification for combining two qimages on top of each other
class IQImageCombiner
{
    CLASS_BOILERPLATE( IQImageCombiner );

public:

    /// render src2 on top of src1
    virtual void
    combine( QImage & src1dst, const QImage & src2 ) = 0;

    /// is this fully opaque combiner? Return true for some extra optimization (e.g. no layers
    /// below this one need to be actually rendered...)
    virtual bool
    isOpaque() { return false; }

    virtual
    ~IQImageCombiner() { }
};

/// This class allows rendering of many raster and vector graphics layers on top of each other.
/// The rendering of the raster layers can be adjusted by specifying IQImageCombiners.
///
/// It is essentially a slightly higher level interface on top of IRemoteVGView, and it
/// inherits its essential limitation: all VG layers are rendered on top of all raster
/// layers. The good news is that it can be efficiently implemented even when VG layers
/// are rendered on the client side.
///
/// Again, this is the lowest level functionality for multiple layers, for example there is
/// no layer deletion, re-ordering, marshalling input, etc.
///
/// \note there is a slightly different implementation of this in LayeredViewArbitrary,
/// which has essentially the same functionality as this class, but it allows layers
/// to have arbitrary order, e.g. raster/VG/raster. See that class's documentation for more
/// relevant info.
///
class LayeredRemoteVGView
    : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( LayeredRemoteVGView );

public:

//    static
//    LayeredRemoteVGView::SharedPtr
//    create( IConnector * connector, QString viewName, QObject * parent = nullptr );

    /// constructor
    LayeredRemoteVGView( IConnector * connector, QString viewName, QObject * parent = nullptr );

    int
    nRasterLayers();

    int
    nVGLayers();

    int
    nLayers();

    void
    resetLayers();

    void
    setRasterLayer( int layer, const QImage & img );

    void
    setRasterLayerCombiner( int layer, IQImageCombiner::SharedPtr combiner );

    void
    setVGLayer( int layer, const VectorGraphics::VGList & vglist );

    QString
    viewName();

    QSize
    getClientSize();

public slots:

    /// schedule a refresh with a given ID
    /// this allows the caller to be notified when the latest painting has reached the client
    qint64
    scheduleRepaint( qint64 id = - 1 );

signals:

    /// this signal is emitted when the master UI changes the size of the view
    void
    sizeChanged();

    /// emitted when client repainted the view
    void
    repainted( qint64 id );

    /// emitted when client sends us a gesture
    void
    inputEvent( InputEvent e );

private:

    IRemoteVGView::UniquePtr m_vgView = nullptr;

    struct RasterLayerInfo {
        QImage qimg;
        IQImageCombiner::SharedPtr combiner = nullptr;
    };

    struct VGLayerInfo {
        VectorGraphics::VGList vglist;
    };

    std::vector < RasterLayerInfo > m_rasterLayers;
    std::vector < VGLayerInfo > m_vgLayers;

    qint64 m_repaintId = - 1;
    QTimer * m_timer = nullptr;

private slots:

    void
    p_timerCB();

    void
    p_sizeChangedCB();
};

/// paints one raster over the other one, overwriting the bottom one
/// it does honor the alphas of the source pixels
//class DefaultCombiner : public IQImageCombiner
//{
//public:

//    virtual void
//    combine( QImage & src1dst, const QImage & src2 ) override
//    {
//        QPainter p( & src1dst );
//        p.drawImage( 0, 0, src2 );
//    }
//};

/// paints one raster over the other one, overwriting the bottom one
/// it does honor the alphas of the source pixels
/// it's also possible to set the alpha for the whole
/// src image
class AlphaCombiner : public IQImageCombiner
{
public:

    AlphaCombiner( double alpha = 1.0 )
    {
        setAlpha( alpha );
    }

    void
    setAlpha( double alpha )
    {
        CARTA_ASSERT( alpha >= 0.0 && alpha <= 1.0 );
        m_alpha = alpha;
    }

    virtual void
    combine( QImage & src1dst, const QImage & src2 ) override
    {
        // small optimization for alpha = 0, and for empty source or destination
        if ( m_alpha == 0.0 || src1dst.size().isEmpty() || src2.size().isEmpty() ) {
            return;
        }
        QPainter p( & src1dst );
        p.setOpacity( m_alpha );
        p.drawImage( 0, 0, src2 );
    }

    virtual bool
    isOpaque()  override
    {
        return m_alpha == 1.0;
    }

private:

    double m_alpha = 1.0;
};

/// the default combiner is the AlphaCombiner
using DefaultCombiner = AlphaCombiner;

/// applies an RGB mask to the source pixels before painting the source image
/// over the destination
/// one can also set alpha for the whole src image
/// painting is done using QPainter::CompositionMode_Plus by default
///
/// So the inputs are:
/// alpha
/// mask
/// composition mode
class PixelMaskCombiner : public IQImageCombiner
{
public:

    void
    setCompositionMode( const QPainter::CompositionMode & mode = QPainter::CompositionMode_Plus )
    {
        m_compositionMode = mode;
    }

    void
    setMask( quint32 mask = DefaultMask )
    {
        m_mask = mask;
    }

    void
    setAlpha( double alpha = 1.0 )
    {
        CARTA_ASSERT( alpha >= 0.0 && alpha <= 1.0 );
        m_alpha = alpha;
    }

    virtual void
    combine( QImage & src1dst, const QImage & src2 ) override
    {
        // small optimization for alpha = 0, and for empty source or destination
        if ( m_alpha == 0.0 || src1dst.size().isEmpty() || src2.size().isEmpty() ) {
            return;
        }

        // make a copy of the source image
        QImage src22 = src2;

        // make sure it's in ARGB32 format
        if ( src22.format() != QImage::Format_ARGB32 ) {
            src22 = src22.convertToFormat( QImage::Format_ARGB32 );
        }

        // apply in-place mask on the copy
        for ( int y = 0 ; y < src2.height() ; y++ ) {
            unsigned char * chr = src22.scanLine( y );
            QRgb * ptr = (QRgb *) ( chr );
            for ( int x = 0 ; x < src22.width() ; x++ ) {
                * ptr = ( * ptr ) & m_mask;
                ptr++;
            }
        }

        // draw the masked copy on top of the source
        QPainter p( & src1dst );
        p.setCompositionMode( m_compositionMode );
        p.setOpacity( m_alpha );
        p.drawImage( 0, 0, src22 );
    } // combine

    static constexpr quint32 DefaultMask = 0xffffffff;

private:

    QPainter::CompositionMode m_compositionMode = QPainter::CompositionMode_Plus;
    quint32 m_mask = DefaultMask;
    double m_alpha = 1.0;
};

/// Similar to LayeredRemoteVGView but the layers can be ordered arbitrarily, e.g.
/// you can have a raster layer above a VG layer.
///
/// \note In order to achieve arbitrary ordering of layers, it is possible some of
/// the vector graphics drawing may have to be done on the server side even if
/// client side rendering is requested, which might result in slower performance in some
/// scenarios.
///
/// \note In the current implementation all of the rendering is done on server.
///
/// \todo Delegate at least the top VG layer rendering to RemoteVGView.
class LayeredViewArbitrary
    : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( LayeredViewArbitrary );

    enum class LayerType
    {
        Raster, VG
    };
    struct LayerInfo {
        LayerType type;
        bool visible = true;
        QImage qimg;
        bool hasTransparentPixels = false;
        IQImageCombiner::SharedPtr comb = nullptr;
        VectorGraphics::VGList vglist;
    };

public:

    /// constructor
    LayeredViewArbitrary( IConnector * connector, QString viewName, QObject * parent = nullptr );

    /// returns the number of layers
    size_t
    nLayers() const;

    void
    removeAllLayers();

    /// \brief setLayerRaster
    /// \param ind index of the layer
    /// \param img the raster image
    /// \param hasTransparentPixels whether the image could have alpha pixels
    ///
    void
    setLayerRaster( int ind, const QImage & img, bool hasTransparentPixels );

    /// use this if you don't know whether the image has transparent pixels
    /// the value will be determined based on format of the image
    void
    setLayerRaster( int layer, const QImage & img );

    /// set the layer's combiner
    void
    setLayerCombiner( int layer, IQImageCombiner::SharedPtr combiner );

    /// set the layer to be a VG layer with the given content
    void
    setLayerVG( int layer, const VectorGraphics::VGList & vglist );

    void
    setLayerVisibility( int layer, bool flag );

    void
    setVGrenderedOnServer( bool flag );

    bool
    isVGrenderedOnServer();

    /// returns true if the layers are organized so that all raster layers
    /// are below all VG layers (i.e. they can be optimally rendered for client side
    /// rendering)
    bool
    isOptimal();

    QString
    viewName();

    QSize
    getClientSize();

public slots:

    /// schedule a refresh with a given ID
    /// this allows the caller to be notified when the latest painting has reached the client
    qint64
    scheduleRepaint( qint64 id = - 1 );

signals:

    /// this signal is emitted when the master UI changes the size of the view
    void
    sizeChanged();

    /// emitted when client repainted the view
    void
    repainted( qint64 id );

    /// emitted when client sends us a gesture
    void
    inputEvent( InputEvent e );

private:

    IRemoteVGView::UniquePtr m_vgView = nullptr;
    std::vector < LayerInfo > m_layers;

    qint64 m_repaintId = - 1;
    QTimer * m_timer = nullptr;

    // helper to create and return reference to a layer
    LayeredViewArbitrary::LayerInfo &
    linfo( int layer );

private slots:

    void
    p_timerCB();

//    void
//    p_sizeChangedCB();
};
}
}
