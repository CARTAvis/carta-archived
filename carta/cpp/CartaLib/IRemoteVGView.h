#pragma once

#include "CartaLib.h"
#include "core/IView.h"
#include "VectorGraphics/VGList.h"

#include <QObject>
#include <QString>
#include <QImage>
#include <QJsonObject>
#include <QJsonDocument>

namespace Carta
{
namespace Lib
{
namespace InputEvents
{
class BaseEvent
{
public:

    BaseEvent( const QJsonObject & json )
    {
        m_json = json;
    }

    const QJsonObject &
    json() const { return m_json; }

private:

    QJsonObject m_json;
};

class TouchEvent
{
public:

    TouchEvent( const BaseEvent & baseEvent )
    {

        if ( baseEvent.json()["type"] != "tap" ) { return; }

        if ( ! baseEvent.json()["x"].isDouble() ) { return; }
        if ( ! baseEvent.json()["y"].isDouble() ) { return; }
        m_pos.rx() = baseEvent.json()["x"].toDouble();
        m_pos.ry() = baseEvent.json()["y"].toDouble();

        m_valid = true;
    }

    const QPointF &
    pos() const { return m_pos; }

    bool
    valid() const
    {
        return m_valid;
    }

private:

    bool m_valid = false;
    QPointF m_pos;
};
/*
template < class Type >
Type *
convertInputEvent( const BaseEvent & baseEvent )
{
    try {
        return new Type( baseEvent );
    }
    catch ( ... ) {
        return nullptr;
    }
}
*/

static int
eventApiTest()
{
    QString s = "{ 'type': 'tap', 'x': 1, 'y': 2.3 }";
    qDebug() << "Testing events" << s.toLatin1();

//    QJsonParseError error;
//    QJsonDocument jdoc = QJsonDocument::fromJson(s.toLatin1(), & error);
//    if( ! jdoc.isObject()) {
//        qCritical() << "json error:" << error.errorString();
//        return 0;
//    }
//    QJsonObject jobj = jdoc.object();

    QJsonObject jobj;
    jobj["type"] = "tap";
    jobj["x"] = 3.1;
    jobj["y"] = 7;

//    std::unique_ptr< BaseEvent > be = new BaseEvent( jobj);
    BaseEvent be( jobj );

    TouchEvent te( be );

//    std::unique_ptr < TouchEvent > te( convertInputEvent < TouchEvent > ( be ) );
    if ( te.valid() ) {
        qDebug() << "Touch event good" << te.pos();
    }
    else {
        qDebug() << "Touch event no good";
    }
    return 1;
} // eventApiTest

static auto xxx = eventApiTest();
}

/// brainstorming:
/// - right now we really only care about users with a keyboard and mouse, but...
/// - we want the ability (in the future) to make the viewer work on mobile devices, which
///   are most likely tablets with touch screens
/// - we want the ability to make the user interface as intuitive as possible on both
///   mobile and desktop
/// - we want to avoid silly emulation of mouse events using touch, because that would not
///   produce intuitive experience for touch users, consider this as an example:
///   - on desktop we pan using a single tap
///   - on desktop we zoom using mouse scroll-wheel
///   - on touch the accepted zoom is via pinch, and pan is by sliding a single finger,
///     and you can seemlessly swith from one to the other (try google maps for example)
///   - how would you emulate the desktop pan/zoom with the touches to produce the
///     expected behavior?
/// - some users will want to assign different gestures to different actions, it would be nice
///   if we could do all of it in javascript on the client side
/// - the VGView will support some basic events that have easy counterparts on both mouse &
///   touch devices
/// - but it'll be easy to extend/override these
/// - if we do want to react to specific events (like pinch, on keyboard), it should be possible
///   to do so, but maybe we should refrain from those as much as possible
///
/// Data associated with events:
/// - point P
/// - scalar S
/// - integer I (we could cram this into S)
/// - boolean B
///
/// single tap: P
/// double tap: P
/// pinch: P,S
/// two finger tap: P, P
/// scroll-wheel: P, S
/// two finger drag: P, P
/// hover: P
/// keyboard: P, I
/// left-click: P, S
/// middle-click: P, S
/// right-click: P, S
/// shift-click: P, S
/// ctrl-alt-shift-right-click: P,S or P,I or P,I,I, or P,I,B,B,B
/// swipe: P, S, S (origin, direction angle, speed)
///
/// avoiding virtual inheritance... passing pointers in signals can be tricky business,
/// because who is the owner (consider two receivers in separate threads, connected using
/// queued connection, who frees up the pointer?). This could probably be resolved using
/// smart (shared?) pointers, but we need to test it.
///
class InputEvent2
{
public:

    enum class Type
    {
        Tap, // e.g. click, or one finger tap
        Press, // e.g. long one finger press, or middle mouse click?
        DoubleTap, // e.g. double click, or double tap of a single finger
        Hover, // e.g. mouse move, or long press and drag? or something stateful?
        Custom
    };

    InputEvent2( Type t, QString ct = QString(), QJsonObject json = QJsonObject() )
    {
        m_type = t;
        m_customType = ct;
        m_json = json;
    }

    Type
    type() const { return m_type; }

    QString
    custom() const { return m_customType; }

    const std::vector < QPointF > &
    points() const { return m_points; }

    const std::vector < float > &
    scalars() const { return m_scalars; }

    const std::vector < int64_t > &
    integers() const { return m_integers; }

    const std::vector < bool > &
    bools() const { return m_bools; }

    /// if we really need to store something else in here?
    const std::vector < char > &
    extraBuff() const { return m_extraBuffer; }

private:

    Type m_type;
    QString m_customType;
    std::vector < QPointF > m_points;
    std::vector < float > m_scalars;
    std::vector < int64_t > m_integers;
    std::vector < bool > m_bools;
    std::vector < char > m_extraBuffer;

    QJsonObject m_json;
};

typedef InputEvents::BaseEvent InputEvent;

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

class IQImageCombiner
{
    CLASS_BOILERPLATE( IQImageCombiner );

public:

    virtual void
    combine( QImage & src1dst, const QImage & src2 ) = 0;

    virtual
    ~IQImageCombiner() { }
};

/*
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
*/

/// lowest level functionality for layered views
/// - allows arbitrary many layers, and ability to associate compositors with each raster
/// layer, that's about it
/// - no layer deletion, re-ordering, funnelling input, etc...
/// this is still in experimental stage actually, we difinitely need input apis for example
/// maybe we want to implement this on top of an existing instance of IRemoteVGView instead
/// of creating our own internally?
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

//    LayerHandle
//    addRasterLayer();

//    LayerHandle
//    addVGLayer();

    QString viewName();

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
