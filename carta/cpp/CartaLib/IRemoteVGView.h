#pragma once

#include "CartaLib.h"
#include "core/IView.h"
#include "VectorGraphics/VGList.h"

#include <QObject>
#include <QString>
#include <QImage>

namespace Carta
{
namespace Lib
{
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

    IRemoteVGView( QObject * parent ) : QObject( parent ) { }

    /// returns size of the view in client (the master's UI)
    virtual const QSize &
    getClientSize() = 0;

    /// returns the unique name of this view
    virtual const QString &
    getRVGViewName() = 0;

    /// sets the raster to be rendered in the view
    virtual void
    setRaster( const QImage & image ) = 0;

    /// sets the VG to be rendered on top of the raster
    virtual void
    setVG( const VGList & vglist ) = 0;

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
};

class IQImageCombiner
{
    CLASS_BOILERPLATE( IQImageCombiner );

public:

    virtual void
    combine( QImage & src1dst, const QImage & src2 ) = 0;

    virtual
    ~IQImageCombiner() { }
};

class LayeredRemoteVGView;

class LayerHandle
{
    CLASS_BOILERPLATE( LayerHandle );

public:

    void
    setRaster( const QImage & );

    void
    setVG( const VectorGraphics::VGList & );

    void
    setRasterCombiner( IQImageCombiner::SharedPtr combiner );

    virtual
    ~LayerHandle() { }
};

/// lowest level functionality for layered views
class LayeredRemoteVGView
    : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( LayeredRemoteVGView );

public:

    static
    LayeredRemoteVGView::SharedPtr
    create( IConnector * connector, QString viewName, QObject * parent = nullptr );

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

//    LayerHandle
//    addRasterLayer();

//    LayerHandle
//    addVGLayer();

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

private:

    /// private constructor to make sure we only create shared pointers of this
    LayeredRemoteVGView( IConnector * connector, QString viewName, QObject * parent = nullptr );

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

    void p_sizeChangedCB();

};

/// paints one raster over the other one, overwriting the bottom one
/// it does honor the alphas of the source pixels
class DefaultCombiner : public IQImageCombiner
{
public:

    virtual void
    combine( QImage & src1dst, const QImage & src2 ) override
    {
        QPainter p( & src1dst );
        p.drawImage( 0, 0, src2 );
    }
};

/// same as DefaultCombiner, but it's possible to set the alpha for the whole
/// src image
class AlphaCombiner : public IQImageCombiner
{
public:

    void
    setAlpha( double alpha )
    {
        CARTA_ASSERT( alpha >= 0.0 && alpha <= 1.0 );
        m_alpha = alpha;
    }

    virtual void
    combine( QImage & src1dst, const QImage & src2 ) override
    {
        QPainter p( & src1dst );
        p.setOpacity( m_alpha );
        p.drawImage( 0, 0, src2 );
    }

private:

    double m_alpha = 1.0;
};

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
        // otherwise we need to
        QImage src22 = src2;
        if ( src22.format() != QImage::Format_ARGB32 ) {
            src22 = src22.convertToFormat( QImage::Format_ARGB32 );
        }
        for ( int y = 0 ; y < src2.height() ; y++ ) {
            unsigned char * chr = src22.scanLine( y );
            QRgb * ptr = (QRgb *) ( chr );
            for ( int x = 0 ; x < src22.width() ; x++ ) {
                * ptr = ( * ptr ) & m_mask;
                ptr++;
            }
        }
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
}
}
