/**
 *
 **/

#include "LayeredViewDemo.h"
#include "core/Globals.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/Regions/IRegion.h"
#include "InteractiveShapes.h"
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QTime>
#include <QVector2D>
#include <QJsonArray>
#include <cmath>
#include <functional>

namespace vge = Carta::Lib::VectorGraphics::Entries;

class EyesLayer : public Carta::Hacks::ManagedLayerBase
{
    Q_OBJECT

public:

    EyesLayer( Carta::Hacks::ManagedLayerView * mlv, QString layerName )
        : Carta::Hacks::ManagedLayerBase( mlv, layerName )
    {
        onResize( { 100, 100 } );
    }

private:

    virtual void
    onInputEvent( Carta::Lib::InputEvent & ev ) override
    {
        qDebug() << "eyes layer received event" << ev.json()["type"];

        // try to convert this to touch event
        Carta::Lib::InputEvents::TouchEvent touch( ev );
        if ( touch.isValid() ) {
            qDebug() << "eyes touch:" << touch.pos();
            m_center = touch.pos();
            rerender();
        }
    }

    virtual void
    onResize( const QSize & size ) override
    {
        m_clientSize = size;
        m_center = QPointF( size.width() / 2.0, size.height() / 2.0 );
        rerender();
    }

    void
    rerender()
    {
        QImage img( m_clientSize, QImage::Format_ARGB32_Premultiplied );
        img.fill( QColor( 0, 128, 0, 128 ) );
        QPainter p( & img );
        p.setBrush( QColor( "red" ) );
        p.setPen( QPen( QColor( "blue" ), 5 ) );
        p.drawEllipse( m_center,
                       40, 60 );
        p.end();
        setRaster( img );
    }

    QPointF m_center;
    QSize m_clientSize = QSize( 100, 100 );
};

class RepelLayer : public Carta::Hacks::ManagedLayerBase
{
    Q_OBJECT

public:

    RepelLayer( Carta::Hacks::ManagedLayerView * mlv, QString layerName )
        : Carta::Hacks::ManagedLayerBase( mlv, layerName )
    {
        onResize( { 100, 100 } );
    }

private:

    virtual void
    onInputEvent( Carta::Lib::InputEvent & ev ) override
    {
        Carta::Lib::InputEvents::HoverEvent hover( ev );
        double speed = 0.01;
        if ( hover.isValid() ) {
            m_center = ( 1 - speed ) * m_center + speed * hover.pos();
            QVector2D dv = QVector2D( m_center ) - QVector2D( hover.pos() );
            double dist = dv.length() / 100.0;
            double dd = 10.0 / ( dist + 1 ) + 10;
            m_size = QSize( dd, dd );

            rerender();
        }
    }

    virtual void
    onResize( const QSize & size ) override
    {
        m_clientSize = size;
        m_center = QPointF( size.width() / 2.0, size.height() / 2.0 );
        rerender();
    }

    void
    rerender()
    {
        Carta::Lib::VectorGraphics::VGComposer comp;

        comp.append < vge::SetPenColor > ( "yellow" );
        comp.append < vge::SetPenWidth > ( 2.0 );
        QRectF rect( m_center, m_size );
        rect.moveCenter( m_center );
        comp.append < vge::DrawRect > ( rect );
        setVG( comp.vgList() );
    }

    QPointF m_center;
    QSizeF m_size = QSize( 10.0, 10.0 );
    QSize m_clientSize = QSize( 100, 100 );
};

class ControlPoint
{
    CLASS_BOILERPLATE( ControlPoint );

public:

    void
    setPosition( const QPointF & pt ) { m_position = pt; }

    const QPointF &
    position() const { return m_position; }

    void
    setMovable( bool flag ) { m_movable = flag; }

    bool
    isMovable() const { return m_movable; }

    void
    setUserData( void * data ) { m_userData = data; }

    void *
    userData() const { return m_userData; }

private:

    QPointF m_position = QPointF( 0.0, 0.0 );
    bool m_movable = true;
    void * m_userData = nullptr;
};

class ControlPointSet : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( ControlPointSet );

public:

    qint64
    addPoint( ControlPoint::SharedPtr pt )
    {
        m_cps.push_back( pt );
        return m_cps.size() - 1;
    }

    const std::vector < ControlPoint::SharedPtr > &
    pts() { return m_cps; }

private:

    std::vector < ControlPoint::SharedPtr > m_cps;
};

class ControlPointLayer : public Carta::Hacks::ManagedLayerBase
{
    Q_OBJECT

public:

    ControlPointLayer( Carta::Hacks::ManagedLayerView * mlv, QString layerName )
        : Carta::Hacks::ManagedLayerBase( mlv, layerName )
    {
        onResize( { 100, 100 } );
    }

    void
    setCPSet( ControlPointSet::SharedPtr set )
    {
        m_cpSet = set;
        rerender();
    }

    ControlPointSet::SharedPtr
    cpSet() { return m_cpSet; }

signals:

private:

    ControlPointSet::SharedPtr m_cpSet = std::make_shared < ControlPointSet > ();
    qint64 m_selectedPointIndex = - 1;

    virtual void
    onInputEvent( Carta::Lib::InputEvent & ev ) override
    {
        Carta::Lib::InputEvents::HoverEvent hover( ev );
        if ( hover.isValid() ) {
            m_selectedPointIndex = - 1;
            const auto & cpArray = m_cpSet-> pts();
            for ( qint64 i = 0 ; i < qint64( cpArray.size() ) ; i++ ) {
                const auto & p = cpArray[i];
                QVector2D dv = QVector2D( p-> position() ) - QVector2D( hover.pos() );
                double dist = dv.length();
                if ( dist < 10 ) {
                    m_selectedPointIndex = i;
                }
            }
            rerender();
        }
    }

    virtual void
    onResize( const QSize & size ) override
    {
        m_clientSize = size;
        rerender();
    } // onResize

    void
    rerender()
    {
        Carta::Lib::VectorGraphics::VGComposer comp;

        comp.append < vge::SetPenColor > ( "red" );
        const auto & cpArray = m_cpSet-> pts();

        for ( qint64 i = 0 ; i < qint64( cpArray.size() ) ; i++ ) {
            if ( i == m_selectedPointIndex ) {
                comp.append < vge::SetPenWidth > ( 2.0 );
            }
            else {
                comp.append < vge::SetPenWidth > ( 1.0 );
            }
            const auto & p = cpArray[i];
            QRectF rect( p-> position(), m_size );
            rect.moveCenter( p-> position() );
            comp.append < vge::DrawRect > ( rect );
        }

        setVG( comp.vgList() );
    } // rerender

    QSizeF m_size = QSize( 10.0, 10.0 );
    QSize m_clientSize = QSize( 100, 100 );
};

namespace editable
{
//class InteractiveShapesController;
class EditableRegionsController;

class EditableControlPoint
    : public InteractiveShapeBase
{
    CLASS_BOILERPLATE( EditableControlPoint );

public:

    EditableControlPoint( std::function < void (bool) > cb )
    {
        m_cb = cb;
    }

    void
    setPos( const QPointF & pt ) { m_pos = pt; }

    const QPointF &
    pos() const { return m_pos; }

    void
    setFillColor( QColor color ) { m_fillColor = color; }

    std::function < void (bool) > m_cb;

    virtual Carta::Lib::VectorGraphics::VGList
    getVGList() override
    {
        if ( ! isActive() ) {
            return Carta::Lib::VectorGraphics::VGList();
        }

        Carta::Lib::VectorGraphics::VGComposer comp;
        QColor color = m_fillColor;
        color.setAlpha( 128 );
        if ( getIsHovered() ) { color.setAlpha( 227 ); }
        if ( getIsSelected() ) { color.setAlpha( 255 ); }
        QRectF rect( m_pos.x() - m_size / 2, m_pos.y() - m_size / 2,
                     m_size, m_size );
        comp.append < vge::FillRect > ( rect, color );
        return comp.vgList();
    }

    virtual bool
    isPointInside( const QPointF & pt ) override
    {
        auto dv = pt - m_pos;
        auto dsq = QPointF::dotProduct( dv, dv );
        return dsq < m_size * m_size;
    }

    virtual void
    handleDragStart( const QPointF & pt ) override
    {
        m_pos = pt;
        m_cb( false );
    }

    virtual void
    handleDrag( const QPointF & pt ) override
    {
        m_pos = pt;
        m_cb( false );
    }

    virtual void
    handleDragDone( const QPointF & pt ) override
    {
        m_pos = pt;
        m_cb( true );
    }

private:

    QPointF m_pos = QPointF( 0, 0 );
    QColor m_fillColor = QColor( 255, 0, 0 );
    double m_size = 7;
};

static QPen shadowPen = QPen( QBrush( QColor( 255, 0, 0, 128 ) ), 3 );
static QBrush shadowBrush = QBrush( QColor( 255, 255, 255, 64 ) );
static QPen outlinePen = QPen( QBrush( QColor( 0, 255, 0, 255 ) ), 1 );
static QBrush controlPointBrush = QBrush( QColor( 255, 255, 0, 255 ) );

class EditableCircle : public InteractiveShapeBase
{
//    Q_OBJECT
    CLASS_BOILERPLATE( EditableCircle );

public:

    EditableCircle( EditableRegionsController & erc, Carta::Lib::Regions::Circle * circleRegion )
        : m_erc( erc )
    {
        m_circleRegion = circleRegion;
    }

    EditableRegionsController & m_erc;

    virtual Carta::Lib::VectorGraphics::VGList
    getVGList() override
    {
        Carta::Lib::VectorGraphics::VGComposer comp;

//        QRectF ( m_center.x() - m_radius, m_center.y() - m_radius,
//                       m_radius * 2, m_radius * 2 );

        if ( getIsHovered() ) {
            double penWidth = 2;

            penWidth = 4;
            comp.append < vge::SetPen > ( QPen( QColor( 255, 255, 255, 128 ), penWidth ) );
            comp.append < vge::DrawEllipse > ( m_circleRegion-> outlineBox() );
        }

        if ( getIsSelected() ) {
            comp.append < vge::SetPen > ( QPen( QColor( 0, 255, 0, 128 ), 1 ) );
            comp.append < vge::DrawRect > ( m_circleRegion->outlineBox() );
        }

        if ( m_inDragMode || isInEditMode() ) {
            comp.append < vge::SetPen > ( QPen( QColor( 0, 255, 255, 128 ), 1 ) );
            comp.append < vge::DrawEllipse > ( m_dragRect );
        }

        if ( isInEditMode() ) {
            comp.append < vge::SetPen > ( QPen( QColor( 255, 0, 0 ), 1 ) );
            comp.append < vge::DrawLine > ( m_centerCP-> pos(), m_radiusCP-> pos() );
        }

        return comp.vgList();
    } // getVGList

    virtual bool
    isPointInside( const QPointF & pt ) override
    {
        return m_circleRegion-> isPointInside( {pt} );
    }

    virtual void
    handleDragStart( const QPointF & pt ) override
    {
        m_inDragMode = true;
        m_dragOffset = pt - m_circleRegion-> center();

//        m_dragRect = m_circleRegion-> outlineBox();

        setDragRect( m_circleRegion-> outlineBox() );
    }

    virtual void
    handleDrag( const QPointF & pt ) override
    {
        // calculate offset if we are starting the drag
        if ( ! m_inDragMode ) {
            qWarning() << "dragging but not in drag mode";
            return;
        }
        auto r = m_dragRect;
        r.translate( pt - m_dragRect.center() - m_dragOffset );
        setDragRect( r );

//        m_dragRect.translate( pt - m_dragRect.center() - m_dragOffset );
    }

    virtual void
    handleDragDone( const QPointF & pt ) override
    {
        Q_UNUSED( pt ); // really?
        m_inDragMode = false;
        auto newCenter = m_dragRect.center();
        m_circleRegion-> setCenter( newCenter );
    }

private:

    void
    setDragRect( const QRectF & r )
    {
        m_dragRect = r;
        if ( m_centerCP ) {
            m_centerCP-> setPos( m_dragRect.center() );
            m_radiusCP-> setPos( QPointF( m_dragRect.right(), m_dragRect.center().y() ) );
        }
    }

    virtual void
    do_editModeChanged() override;

    void
    controlPointCB( bool movingCenter, bool final );

    Carta::Lib::Regions::Circle * m_circleRegion = nullptr;
    QPointF m_dragOffset = QPointF( 0, 0 );
    QRectF m_dragRect;
    bool m_inDragMode = false;

    EditableControlPoint::SharedPtr m_centerCP = nullptr;
    EditableControlPoint::SharedPtr m_radiusCP = nullptr;
};

class EditablePolygon : public InteractiveShapeBase
{
    CLASS_BOILERPLATE( EditablePolygon );

public:

    EditablePolygon( EditableRegionsController & erc, Carta::Lib::Regions::Polygon * polygonRegion )
        : m_erc( erc )
    {
        m_polygonRegion = polygonRegion;
        m_shadowPolygon = m_polygonRegion-> qpolyf();
        CARTA_ASSERT( m_shadowPolygon.size() > 0 );

//        m_firstPtOriginal = m_shadowPolygon.at( 0 );
    }

//    QPointF m_firstPtOriginal = QPointF(0,0);
    QPointF m_dragStartPt = QPointF( 0, 0 );

    virtual Carta::Lib::VectorGraphics::VGList
    getVGList() override
    {
        Carta::Lib::VectorGraphics::VGComposer comp;

        bool drawShadow = false;
        QPen pen = shadowPen;
        QBrush brush = Qt::NoBrush;

        if ( getIsHovered() ) {
            drawShadow = true;
        }

        if ( getIsSelected() ) {
            drawShadow = true;
            comp.append < vge::SetPen > ( outlinePen );
            comp.append < vge::SetBrush > ( QBrush( Qt::NoBrush ) );
            comp.append < vge::DrawRect > ( m_shadowPolygon.boundingRect() );
        }

        if ( m_inDragMode || isInEditMode() ) {
            drawShadow = true;
            brush = shadowBrush;

//            comp.append < vge::SetPen > ( QPen( QColor( 0, 255, 255, 128 ), 1 ) );
//            auto poly = m_polygonRegion-> qpolyf();
//            poly.translate( m_dragCenter - poly.boundingRect().center() );
//            comp.append < vge::DrawPolygon > ( m_shadowPolygon );
        }

        if ( drawShadow ) {
            comp.append < vge::SetPen > ( pen );
            comp.append < vge::SetBrush > ( brush );
            comp.append < vge::DrawPolygon > ( m_shadowPolygon );
        }

        if ( isInEditMode() ) {
//            comp.append < vge::SetPen > ( QPen( QColor( 255, 0, 0 ), 1 ) );

//            comp.append < vge::DrawLine > ( m_centerCP-> pos(), m_radiusCP-> pos() );
        }

        return comp.vgList();
    } // getVGList

    virtual bool
    isPointInside( const QPointF & pt ) override
    {
        return m_polygonRegion-> isPointInside( {pt} );
    }

    virtual void
    handleDragStart( const QPointF & pt ) override
    {
        m_inDragMode = true;
        m_dragStartPt = pt;
    }

    virtual void
    handleDrag( const QPointF & pt ) override
    {
        // calculate offset if we are starting the drag
        if ( ! m_inDragMode ) {
            qWarning() << "dragging but not in drag mode";
            return;
        }

        // calculate offset to move the shadow polygon to match the un-edited shape
        QPointF offset = m_polygonRegion->qpolyf().at( 0 ) - m_shadowPolygon.at( 0 );

        // now move it by the amount of drag
        offset += pt - m_dragStartPt;

        // apply the offset to the shadow
        m_shadowPolygon.translate( offset );
        syncShadowToCPs();
    } // handleDrag

    virtual void
    handleDragDone( const QPointF & pt ) override
    {
        // calculate offset to move the shadow polygon to match the un-edited shape
        QPointF offset = m_polygonRegion->qpolyf().at( 0 ) - m_shadowPolygon.at( 0 );

        // now move it by the amount of drag
        offset += pt - m_dragStartPt;

        // apply the offset to the shadow
        m_shadowPolygon.translate( offset );
        syncShadowToCPs();

        // exit drag mode
        m_inDragMode = false;

        // update the region
        m_polygonRegion-> setqpolyf( m_shadowPolygon );
    } // handleDragDone

    void
    syncShadowToCPs();

private:

    EditableRegionsController & m_erc;

    virtual void
    do_editModeChanged() override;

    void
    controlPointCB( int index, bool final );

    /// @todo: make this a reference to signify non-ownership?
    Carta::Lib::Regions::Polygon * m_polygonRegion = nullptr;
    bool m_inDragMode = false;

    // the shadow polygon
    QPolygonF m_shadowPolygon;

    std::vector < EditableControlPoint::SharedPtr > m_cps;
};

typedef Carta::Lib::Regions::RegionBase RegionSet;

typedef qint64 SourceID;

/// container for region set, capable of signaling changes
class RegionSetModel : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( RegionSetModel );

public:

    RegionSetModel()
    {
//        connect( this, & Me::privateChangedSignal,
//                 this, & Me::privateChangedSignalCB,
//                 Qt::QueuedConnection );
        m_updateTimer.setInterval( 1 );
        m_updateTimer.setSingleShot( true );
        connect( & m_updateTimer, & QTimer::timeout,
                 this, & Me::updateTimerCB );
    }

    virtual
    ~RegionSetModel() { }

    RegionSet::SharedPtr
    regionSet() { return m_regionSet; }

    void
    setRegionSet( RegionSet::SharedPtr regionSet )
    {
        m_regionSet = regionSet;
    }

    /// schedule an update signal with the selected source id
    void
    scheduleUpdateSignal( SourceID sourceID = - 1 )
    {
        m_lastSourceID = sourceID;
        if ( ! m_updateTimer.isActive() ) {
            m_updateTimer.start();
        }
    }

signals:

    void
    changed( SourceID sourceID );

private slots:

    void
    updateTimerCB()
    {
        emit changed( m_lastSourceID );
    }

private:

    RegionSet::SharedPtr m_regionSet = nullptr;
    QTimer m_updateTimer;
    SourceID m_lastSourceID = 0;
};

static SourceID
getUniqueSourceID()
{
    static SourceID m_counter = 0;
    return m_counter++;
}

/// Purpose of this class is to provide easy interactive editing on regions.
class EditableRegionsController : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( EditableRegionsController );

public:

    EditableRegionsController()
    {
        m_editableShapesController.reset( new InteractiveShapesController );
    }

    // get the current vg list
    Carta::Lib::VectorGraphics::VGList
    vgList()
    {
        // get the raw regions graphics
        Carta::Lib::VectorGraphics::VGList vgList;
        if ( m_regionSet ) {
            vgList = m_regionSet-> vgList();
        }

        // get the editable shapes graphics
        Carta::Lib::VectorGraphics::VGList
            editableShapesVGList = m_editableShapesController-> vgList();

        // concatenate the two vg lists
        Carta::Lib::VectorGraphics::VGComposer comp;
        comp.append < vge::Save > ();
        comp.appendList( vgList );
        comp.append < vge::Restore > ();
        comp.appendList( editableShapesVGList );
        return comp.vgList();
    } // vgList

    /// set the region set on which this layer will operate
    void
    setRegionSet( RegionSet::SharedPtr regionSet )
    {
        m_regionSet = regionSet;

        m_editableShapesController-> reset();

        // now go through the regions and create an editable shape for each
        traverse( m_regionSet.get() );
    }

    /// get the region set model on which we are operating
    RegionSet::SharedPtr
    getRegionSet()
    {
        return m_regionSet;
    }

    // after an action is performed, this should be called to see if new VG is available
    bool
    hasNewVG()
    {
        return m_newVGflag;
    }

    // reset the new vg flag
    void
    resetNewVG()
    {
        m_newVGflag = false;
    }

    bool
    wasRegionSetUpdated()
    {
        return m_regionSetUpdatedFlag;
    }

    void
    resetRegionSetUpdated()
    {
        m_regionSetUpdatedFlag = false;
    }

    void
    handleEvent( Carta::Lib::InputEvents::JsonEvent & ev )
    {
        m_editableShapesController-> handleEvent( ev );
    }

    virtual
    ~EditableRegionsController() { }

    // get a reference to editable shapes controller
    InteractiveShapesController &
    editableShapesController()
    {
        return * m_editableShapesController;
    }

private:

    // recursively visit the list of nodes in the regions under this node
    // and add their corresponding shapes to the editable shapes controller
    void
    traverse( Carta::Lib::Regions::RegionBase * node )
    {
        if ( ! node ) {
            return;
        }

        // traverse the kids first in reverse order
        auto & children = node-> children();

//        for ( auto i = children.rbegin() ; i != children.rend() ; ++i ) {
//            traverse( * i );
//        }

        for ( auto i = children.begin() ; i != children.end() ; ++i ) {
            traverse( * i );
        }

        // now that we have processed the kids, process this region itself
        if ( node->typeName() == Carta::Lib::Regions::Circle::TypeName ) {
            Carta::Lib::Regions::Circle * circle =
                dynamic_cast < Carta::Lib::Regions::Circle * > ( node );
            CARTA_ASSERT( circle );

            auto circleShape = std::make_shared < EditableCircle > ( * this, circle );
            m_editableShapesController-> addShape( circleShape );
        }
        else if ( node->typeName() == Carta::Lib::Regions::Polygon::TypeName ) {
            Carta::Lib::Regions::Polygon * polygon =
                dynamic_cast < Carta::Lib::Regions::Polygon * > ( node );
            CARTA_ASSERT( polygon );

            auto polygonShape = std::make_shared < EditablePolygon > ( * this, polygon );
            m_editableShapesController-> addShape( polygonShape );
        }
    } // traverse

    bool m_newVGflag = true;
    bool m_regionSetUpdatedFlag = true;

    /// pointer to editable shapes controller
    InteractiveShapesController::UniquePtr m_editableShapesController = nullptr;

    /// pointer to region set model
    RegionSet::SharedPtr m_regionSet = nullptr;

    /// unique source ID for interacting with region set model
//    SourceID m_mySourceID = getUniqueSourceID();
};

/// our hacky managed layer implementation that is capable of editing
/// a region set
///
/// All actual work is done via EditableRegionsController
class RegionEditorLayer : public Carta::Hacks::ManagedLayerBase
{
    Q_OBJECT
    CLASS_BOILERPLATE( RegionEditorLayer );

public:

    RegionEditorLayer( Carta::Hacks::ManagedLayerView * mlv, QString layerName )
        : Carta::Hacks::ManagedLayerBase( mlv, layerName )
    {
        m_editableRegionsController.reset( new EditableRegionsController );
    }

    /// set the region set model on which this layer will operate
    void
    setRegionSetModel( RegionSetModel::SharedPtr regionSetModel )
    {
        m_regionSetModel = regionSetModel;

        // disconnect the previous model (if any)
        if ( m_regionSetModelConnection ) {
            disconnect( m_regionSetModelConnection );
        }

        // connect the new model
        m_regionSetModelConnection =
            connect( regionSetModel.get(), & RegionSetModel::changed,
                     this, & Me::regionSetModelCB );

        // tell the editable regions controller we have a new region set
        m_editableRegionsController-> setRegionSet( m_regionSetModel-> regionSet() );
    }

    /// get the region set model on which we are operating
    RegionSetModel::SharedPtr
    regionSetModel()
    {
        return m_regionSetModel;
    }

signals:

private slots:

    // callback for region set model
    void
    regionSetModelCB( SourceID id )
    {
        // if we originated this update, ignore it
        if ( id == m_sourceId ) { return; }

        // otherwise tell the controller about the new region set
        m_editableRegionsController-> setRegionSet( m_regionSetModel-> regionSet() );
    }

private:

    virtual void
    onResize( const QSize & size ) override
    {
        Q_UNUSED( size );
        rerender();
    }

    void
    rerender()
    {
        // display whatever the editable regions controller displays
        Carta::Lib::VectorGraphics::VGList vgList
            = m_editableRegionsController-> vgList();

        setVG( vgList );
    }

    virtual void
    onInputEvent( Carta::Lib::InputEvent & ev ) override
    {
        // deliver the event to editable regions controller
        m_editableRegionsController-> handleEvent( ev );

        // check if we need to update vg
        if ( m_editableRegionsController-> hasNewVG() ) {
            setVG( m_editableRegionsController-> vgList() );
        }

        // was the region set updated? if yes, let the region set model signal the update
        if ( m_editableRegionsController-> wasRegionSetUpdated() ) {
            // set the source ID to us, so we can ignore this update
            m_regionSetModel-> scheduleUpdateSignal( m_sourceId );
        }
    } // onInputEvent

    RegionSetModel::SharedPtr m_regionSetModel = nullptr;
    QMetaObject::Connection m_regionSetModelConnection;

    EditableRegionsController::UniquePtr m_editableRegionsController = nullptr;
    SourceID m_sourceId = getUniqueSourceID();
};

class EditableLayer : public Carta::Hacks::ManagedLayerBase
{
    Q_OBJECT
    CLASS_BOILERPLATE( EditableLayer );

public:

    EditableLayer( Carta::Hacks::ManagedLayerView * mlv, QString layerName )
        : Carta::Hacks::ManagedLayerBase( mlv, layerName )
    {
        onResize( QSize( 100, 100 ) );
    }

    void
    setCPSet( ControlPointSet::SharedPtr set )
    {
        m_cpSet = set;
        rerender();
    }

    ControlPointSet::SharedPtr
    cpSet() { return m_cpSet; }

signals:

private:

    ControlPointSet::SharedPtr m_cpSet = std::make_shared < ControlPointSet > ();
    qint64 m_selectedPointIndex = - 1;

    virtual void
    onInputEvent( Carta::Lib::InputEvent & ev ) override
    {
        Carta::Lib::InputEvents::HoverEvent hover( ev );
        if ( hover.isValid() ) {
            m_selectedPointIndex = - 1;
            const auto & cpArray = m_cpSet-> pts();
            for ( qint64 i = 0 ; i < qint64( cpArray.size() ) ; i++ ) {
                const auto & p = cpArray[i];
                QVector2D dv = QVector2D( p-> position() ) - QVector2D( hover.pos() );
                double dist = dv.length();
                if ( dist < 10 ) {
                    m_selectedPointIndex = i;
                }
            }
            rerender();
        }
    }

    virtual void
    onResize( const QSize & size ) override
    {
        m_clientSize = size;
        rerender();
    } // onResize

    void
    rerender()
    {
        Carta::Lib::VectorGraphics::VGComposer comp;

        comp.append < vge::SetPenColor > ( "red" );
        const auto & cpArray = m_cpSet-> pts();

        for ( qint64 i = 0 ; i < qint64( cpArray.size() ) ; i++ ) {
            if ( i == m_selectedPointIndex ) {
                comp.append < vge::SetPenWidth > ( 2.0 );
            }
            else {
                comp.append < vge::SetPenWidth > ( 1.0 );
            }
            const auto & p = cpArray[i];
            QRectF rect( p-> position(), m_size );
            rect.moveCenter( p-> position() );
            comp.append < vge::DrawRect > ( rect );
        }

        setVG( comp.vgList() );
    } // rerender

    QSizeF m_size = QSize( 10.0, 10.0 );
    QSize m_clientSize = QSize( 100, 100 );
};

void
EditableCircle::do_editModeChanged()
{
    qDebug() << "Circle is" << ( isInEditMode() ? "in" : "not in" ) << "edit mode";

    // make the control points
    if ( ! m_centerCP ) {
        auto centercb = [&] ( bool final ) {
            controlPointCB( true, final );
        };
        m_centerCP = std::make_shared < EditableControlPoint > ( centercb );
        auto radiuscb = [&] ( bool final ) {
            controlPointCB( false, final );
        };
        m_radiusCP = std::make_shared < EditableControlPoint > ( radiuscb );

//        m_radiusCP-> setFillColor( "blue");
        m_erc.editableShapesController().addShape( m_centerCP );
        m_erc.editableShapesController().addShape( m_radiusCP );
    }

    // update current positions of CPs
    m_centerCP-> setPos( m_circleRegion->center() );
    m_radiusCP-> setPos( m_circleRegion->center() + QPointF( m_circleRegion->radius(), 0 ) );

    // activate/deactive them based on edit status
    m_centerCP-> setIsActive( isInEditMode() );
    m_radiusCP-> setIsActive( isInEditMode() );
} // do_editModeChanged

void
EditableCircle::controlPointCB( bool movingCenter, bool final )
{
    // if we are moving the center, we first move the radius CP to keep the original radius
    if ( movingCenter ) {
        auto oldCenter = m_dragRect.center();
        auto diff = m_radiusCP-> pos() - oldCenter;
        m_radiusCP-> setPos( m_centerCP-> pos() + diff );
    }

    auto dv = m_radiusCP-> pos() - m_centerCP-> pos();
    double dsq = QPointF::dotProduct( dv, dv );
    double dist = std::sqrt( dsq );

    QRectF r = QRectF( m_centerCP->pos().x() - dist,
                       m_centerCP->pos().y() - dist, dist * 2, dist * 2 );
    r = r.normalized();

    setDragRect( r );

    if ( final ) {
        m_circleRegion->setCenter( m_centerCP-> pos() );
        m_circleRegion->setRadius( dist );
    }
} // controlPointCB

void
EditablePolygon::syncShadowToCPs()
{
    // make missing control points if necessary
    while ( int ( m_cps.size() ) < m_shadowPolygon.size() ) {
        int index = m_cps.size();
        auto cb = [this, index] ( bool final ) {
            controlPointCB( index, final );
        };
        auto cp = std::make_shared < EditableControlPoint > ( cb );
        cp-> setIsActive( false );
        m_cps.push_back( cp );
        m_erc.editableShapesController().addShape( cp );
    }

    //        // make the control points if we have not made them yet
    //        if ( m_cps.size() == 0 ) {
    //            for ( int index = 0 ; index < m_shadowPolygon.size() ; index++ ) {
    //                auto cb = [this, index] ( bool final ) {
    //                    controlPointCB( index, final );
    //                };
    //                auto cp = std::make_shared < EditableControlPoint > ( cb );
    //                m_cps.push_back( cp );
    //                m_erc.editableShapesController().addShape( cp );
    //            }
    //        }

    CARTA_ASSERT( m_shadowPolygon.size() == int ( m_cps.size() ) );
    for ( int index = 0 ; index < m_shadowPolygon.size() ; index++ ) {
        m_cps[index]->setPos( m_shadowPolygon[index] );
    }
} // syncShadowToCPs

void
EditablePolygon::do_editModeChanged()
{
    // update the positions of control points
    syncShadowToCPs();

    // activate/deactivate control points based on edit status
    for ( auto & pt : m_cps ) {
        pt-> setIsActive( isInEditMode() );
    }
} // do_editModeChanged

void
EditablePolygon::controlPointCB( int index, bool final )
{
    auto & poly = m_shadowPolygon;
    if ( index >= 0 && index < poly.size() ) {
        poly[index] = m_cps[index]-> pos();
    }

    if ( final ) {
        m_polygonRegion->setqpolyf( m_shadowPolygon );
    }

//    auto poly = m_polygonRegion-> qpolyf();
//    if ( index >= 0 && index < poly.size() ) {
//        poly[index] = m_cps[index]-> pos();
//        m_polygonRegion-> setqpolyf( poly );
//    }
}

// controlPointCB
}

class ClockLayer : public Carta::Hacks::ManagedLayerBase
{
    Q_OBJECT

public:

    ClockLayer( Carta::Hacks::ManagedLayerView * mlv, QString layerName )
        : Carta::Hacks::ManagedLayerBase( mlv, layerName )
    {
        m_timer.setInterval( 1000 );
        m_timer.start();
        connect( & m_timer, & QTimer::timeout, this, & ClockLayer::timerCB );
        onResize( { 100, 100 } );
    }

private:

    virtual void
    onInputEvent( Carta::Lib::InputEvent & ev ) override
    {
        qDebug() << "Bouncy layer received event" << ev.json()["type"];
        Carta::Lib::InputEvents::TouchEvent touch( ev );
        if ( touch.isValid() ) {
            qDebug() << "Bouncy touch:" << touch.pos();
            m_center = touch.pos();
            rerender();
        }
    }

    virtual void
    onResize( const QSize & size ) override
    {
        m_clientSize = size;
        m_center = QPointF( size.width() - m_radius, size.height() - m_radius );
        rerender();
    }

    void
    rerender()
    {
        QTime time = QTime::currentTime();
        QColor shadow = QColor( 0, 0, 0, 192 );
        QImage img( m_clientSize, QImage::Format_ARGB32_Premultiplied );
        img.fill( QColor( 128, 0, 0, 128 ) );
        QPainter p( & img );
        p.setRenderHint( QPainter::Antialiasing, true );
        p.setBrush( QColor( "black" ) );
        p.setPen( QPen( QColor( "white" ), 2 ) );
        p.drawEllipse( m_center, m_radius, m_radius );
        p.setPen( QPen( QColor( "white" ), 2 ) );
        double tickLenght = 5;
        for ( int h = 0 ; h < 12 ; h++ ) {
            double alpha = h * M_PI * 2 / 12;
            p.drawLine( a2p( alpha, m_radius - tickLenght - ( h % 3 == 0 ? 2 : 0 ) ),
                        a2p( alpha, m_radius ) );
        }
        double hour = time.hour() % 12 + time.minute() / 60.0 + time.second() / 3600.0;
        p.setPen( QPen( QColor( "white" ), 4 ) );
        p.drawLine( m_center, a2p( hour / 12.0 * M_PI * 2, m_radius / 2 ) );
        double minute = time.minute() + time.second() / 60.0;
        p.setPen( QPen( shadow, 4 ) );
        p.drawLine( m_center, a2p( minute / 60.0 * M_PI * 2, m_radius * 3 / 4 ) );
        p.setPen( QPen( QColor( "white" ), 3 ) );
        p.drawLine( m_center, a2p( minute / 60.0 * M_PI * 2, m_radius * 3 / 4 ) );
        p.setPen( QPen( shadow, 2 ) );
        p.drawLine( m_center, a2p( time.second() / 60.0 * M_PI * 2, m_radius - 2 ) );
        p.setPen( QPen( QColor( "yellow" ), 1 ) );
        p.drawLine( m_center, a2p( time.second() / 60.0 * M_PI * 2, m_radius - 2 ) );
        p.end();
        setRaster( img );
    } // rerender

    static QVector2D
    a2v( double a )
    {
        return QVector2D( sin( a ), - cos( a ) );
    }

    QPointF
    a2p( double alpha, double radius )
    {
        return ( QVector2D( m_center ) + radius * a2v( alpha ) ).toPointF();
    }

    void
    timerCB()
    {
        rerender();
    }

    QPointF m_center;
    QSize m_clientSize;
    double m_radius = 50;
    QTimer m_timer;
};

class LayeredViewController : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( LayeredViewController );

public:

    LayeredViewController( Carta::Hacks::ManagedLayerView * mlv,
                           QObject * parent = nullptr ) : QObject( parent )
    {
        m_mlv = mlv;

        // monitor changes to ManagedLayerView so that we can update the UI
        connect( m_mlv, & Carta::Hacks::ManagedLayerView::layersUpdated,
                 this, & Me::mlvUpdatedCB );

        // listen for commands
        using namespace std::placeholders;
        Globals::instance()->connector()->addCommandCallback(
            QString( "/hacks/LayeredViewController/%1/command" ).arg( m_mlv->viewName() ),
            std::bind( & Me::commandCB, this, _1, _2, _3 ) );

        // manually invoke mlvUpdatedCB to update the UI on the client
        mlvUpdatedCB();
    }

    virtual
    ~LayeredViewController() { }

private slots:

    void
    mlvUpdatedCB()
    {
        // get the list of layers from the view
        auto & layers = m_mlv-> layers();
        QJsonArray ja;
        for ( auto layer : layers ) {
            QJsonObject jo;
            jo["name"] = layer-> layerName();
            jo["id"] = layer-> layerID();
            jo["input"] = layer-> hasInput();
            ja.push_back( jo );
        }
        QJsonObject job;
        job["list"] = ja;
        QJsonDocument doc( job );
        QByteArray jstring = doc.toJson();
        qDebug() << "================ JSON ================";
        qDebug() << jstring;
        qDebug() << "======================================";

        Globals::instance()->connector()->setState(
            QString(
                "/hacks/LayeredViewController/%1" ).arg(
                m_mlv-> viewName() ), jstring );
    } // mlvUpdatedCB

private:

    QString
    commandCB( const QString & /*cmd*/, const QString & params, const QString & /*sessionId*/ )
    {
        QJsonDocument doc = QJsonDocument::fromJson( params.toLatin1() );
        if ( ! doc.isObject() ) { return ""; }
        QJsonObject rootObj = doc.object();
        QString cmd = rootObj["command"].toString();
        QJsonValue data = rootObj["data"];
        if ( cmd == "setSelection" ) {
            if ( ! data.isArray() ) { return ""; }
            QJsonArray arr = data.toArray();
            std::vector < Carta::Hacks::ManagedLayerBase::ID > selection;
            for ( auto val : arr ) {
                qDebug() << "Selection" << val.toInt();
                selection.push_back( val.toInt() );
            }

            m_mlv->setInputLayers( selection );
        }
        else if ( cmd == "up" ) {
            if ( ! data.isArray() ) { return ""; }
            QJsonArray arr = data.toArray();
            std::vector < Carta::Hacks::ManagedLayerBase::ID > selection;
            for ( auto val : arr ) {
                selection.push_back( val.toInt() );
            }
            m_mlv-> moveLayersUp( selection );
        }
        else if ( cmd == "down" ) {
            if ( ! data.isArray() ) { return ""; }
            QJsonArray arr = data.toArray();
            std::vector < Carta::Hacks::ManagedLayerBase::ID > selection;
            for ( auto val : arr ) {
                selection.push_back( val.toInt() );
            }
            m_mlv-> moveLayersDown( selection );
        }
        else if ( cmd == "delete" ) {
            if ( ! data.isArray() ) { return ""; }
            QJsonArray arr = data.toArray();
            std::vector < Carta::Hacks::ManagedLayerBase::ID > selection;
            for ( auto val : arr ) {
                selection.push_back( val.toInt() );
            }
            m_mlv-> removeLayers( selection );
        }
        else {
            qWarning() << "Unknown command" << cmd;
        }

        return "";
    } // commandCB

    QString
    setSelectionCB( const QString & /*cmd*/, const QString & params, const QString & /*sessionId*/ )
    {
        QJsonDocument doc = QJsonDocument::fromJson( params.toLatin1() );
        if ( ! doc.isArray() ) { return ""; }
        QJsonArray arr = doc.array();
        std::vector < Carta::Hacks::ManagedLayerBase::ID > selection;
        for ( auto val : arr ) {
            qDebug() << "Selection" << val.toInt();
            selection.push_back( val.toInt() );
        }

        m_mlv->setInputLayers( selection );

        return "";
    }

    Carta::Hacks::ManagedLayerView * m_mlv = nullptr;
};

namespace Carta
{
namespace Hacks
{
struct LayeredViewDemo::Pimpl {
    LayeredViewController::UniquePtr lvc = nullptr;
    ManagedLayerView::UniquePtr mlv = nullptr;
};

LayeredViewDemo::LayeredViewDemo( QObject * parent ) : QObject( parent )
{
    m_pimpl = new Pimpl;

    m_pimpl-> mlv.reset( new ManagedLayerView( "mlv1", Globals::instance()->connector(), this ) );

    auto mlvRaw = m_pimpl-> mlv.get();

//    EyesLayer * eyes1 = new EyesLayer( mlvRaw, "ellipse1" );
    ClockLayer * clockLayer = new ClockLayer( mlvRaw, "clock" );
    Q_UNUSED( clockLayer);

//    RepelLayer * repel1 = new RepelLayer( mlvRaw, "Repel1" );
//    EyesLayer * eyes2 = new EyesLayer( mlvRaw, "ellipse2" );

    /*
    ControlPointLayer * cplayer = new ControlPointLayer( mlvRaw, "cplayer" );
    ControlPointSet::SharedPtr cpset = std::make_shared < ControlPointSet > ();
    ControlPoint::SharedPtr cp1;
    cp1 = std::make_shared < ControlPoint > ();
    cp1-> setPosition( QPointF( 10, 20 ) );
    cpset->addPoint( cp1 );
    cp1 = std::make_shared < ControlPoint > ();
    cp1-> setPosition( QPointF( 30, 10 ) );
    cpset->addPoint( cp1 );
    cplayer-> setCPSet( cpset );
*/

    // ==================================================================================
    // region demo
    // ==================================================================================

    struct RegionDemo {
        editable::RegionSet::SharedPtr regionSet = nullptr;
        editable::RegionSetModel::SharedPtr regionSetModel = nullptr;
    };

    // intentional memory leak to make sure our smart pointers don't go out of scope
    RegionDemo & regionDemo = * new RegionDemo;

    {
        // read in a region from a fixed filename
        QString inputFname = "/scratch/regions.json";
        qDebug() << "Trying to parse" << inputFname;
        QFile fp( inputFname );
        if ( ! fp.open( QIODevice::ReadOnly ) ) {
            qWarning() << "Could not open " << fp.fileName();
        }
        else {
            QByteArray contents = fp.readAll();
            QJsonParseError jerr;
            QJsonDocument jdoc = QJsonDocument::fromJson( contents, & jerr );
            if ( jerr.error != QJsonParseError::NoError ) {
                qWarning() << "Json parse error@" << jerr.offset << ":" << jerr.errorString();
            }
            else {
                Carta::Lib::Regions::RegionBase * bb = Carta::Lib::Regions::fromJson( jdoc.object() );
                regionDemo.regionSet.reset( bb );
                qDebug() << "Read in region base";
            }
        }
    }
    regionDemo.regionSetModel =
        std::make_shared < editable::RegionSetModel > ();

    editable::RegionEditorLayer * reditorlayer = new editable::RegionEditorLayer( mlvRaw, "regions" );
    reditorlayer-> setRegionSetModel( regionDemo.regionSetModel );

    // update region set model with our region set
    regionDemo.regionSetModel-> setRegionSet( regionDemo.regionSet );
    regionDemo.regionSetModel-> scheduleUpdateSignal();

    m_pimpl-> mlv-> setInputLayers( { reditorlayer-> layerID() }
                                    );

    m_pimpl-> lvc.reset( new LayeredViewController( m_pimpl-> mlv.get() ) );
}

LayeredViewDemo::~LayeredViewDemo()
{
    if ( m_pimpl ) {
        delete m_pimpl;
        m_pimpl = nullptr;
    }
}
}
}

// hack for declaring qobject classes inside .cpp instead of headers. This will force
// moc to process the .cpp file...
#include "LayeredViewDemo.moc"
