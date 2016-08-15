/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/InputEvents.h"
#include "CartaLib/VectorGraphics/VGList.h"

namespace editable
{
//class IInteractive
//{
//public:

//    virtual void
//    handleHoverEvent( Carta::Lib::InputEvents::HoverEvent & ev ) = 0;

//    virtual void
//    handleDragStartEvent( Carta::Lib::InputEvents::DragStartEvent & ev ) = 0;

//    virtual void
//    handleDragDoneEvent( Carta::Lib::InputEvents::DragDoneEvent & ev ) = 0;

//    virtual void
//    handleDragEvent( Carta::Lib::InputEvents::DragEvent & ev ) = 0;
//};

class InteractiveShapeBase
{
    CLASS_BOILERPLATE( InteractiveShapeBase );

public:

    /// in order to deliver events to shapes, we need to know whether a point
    /// is inside the shape or not
    virtual bool
    isPointInside( const QPointF & pt )
    {
        Q_UNUSED( pt );
        return false;
    }

    /// it is possible to deactivate a shape so it does not participate in
    /// event delivery
    bool m_isActive = true;
    bool
    isActive() const { return m_isActive; }

    void
    setIsActive( bool flag ) { m_isActive = flag; }

    /// each shape needs to decide how to handle events
    virtual void
    handleEvent( Carta::Lib::InputEvents::JsonEvent & ev );

    void * m_userData = nullptr;
    void *
    getUserData() const
    {
        return m_userData;
    }

    void
    setUserData( void * value )
    {
        m_userData = value;
    }

    bool m_isSelected = false;
    bool
    getIsSelected() const
    {
        return m_isSelected;
    }

    void
    setIsSelected( bool value )
    {
        m_isSelected = value;
    }

    bool m_canDelete = false;
    bool
    getCanDelete() const
    {
        return m_canDelete;
    }

    void
    setCanDelete( bool value )
    {
        m_canDelete = value;
    }

    QString m_cursor;
    QString
    getCursor() const
    {
        return m_cursor;
    }

    void
    setCursor( const QString & value )
    {
        m_cursor = value;
    }

    bool m_isHovered = false;
    bool
    getIsHovered() const
    {
        return m_isHovered;
    }

    /// sets the hovered property
    /// \param value new value
    /// \return whether the underlying VG graphics need to be updated
    /// Default is to return false. Reimplement if the shape needs to be redrawn as
    /// a result of changing hover state.
    virtual bool
    setIsHovered( bool value )
    {
        m_isHovered = value;
        return false;
    }

    virtual Carta::Lib::VectorGraphics::VGList
    getVGList() { return Carta::Lib::VectorGraphics::VGList(); }

//    bool m_isMovable = false;
//    bool
//    getIsMovable() const
//    {
//        return m_isMovable;
//    }

//    void
//    setIsMovable( bool value )
//    {
//        m_isMovable = value;
//    }

    virtual void
    handleDragStart( const QPointF & pt ) { Q_UNUSED( pt ); }

    virtual void
    handleDrag( const QPointF & pt )
    {
        Q_UNUSED( pt );
    }

    virtual void
    handleDragDone( const QPointF & pt ) { Q_UNUSED( pt ); }

    bool m_inEditMode = false;
    bool
    isInEditMode() const { return m_inEditMode; }

    void
    setEditMode( bool flag )
    {
        bool oldFlag = m_inEditMode;
        m_inEditMode = flag;
        if ( oldFlag != flag ) {
            do_editModeChanged();
        }
    }

    /// reimplement this if your shape needs to know whether edit mode changed
    /// default implementation does nothing
    virtual void
    do_editModeChanged()
    {
        ;
    }
};

/// can
class InteractiveShapesController

//        : public QObject
{
//    Q_OBJECT
    CLASS_BOILERPLATE( InteractiveShapesController );

public:

//    InteractiveShapesController( QObject * parent = nullptr ) : QObject( parent ) { }
    InteractiveShapesController() { }

    /// add a shape to the end of the list
    void
    addShape( InteractiveShapeBase::SharedPtr editableShape )
    {
        m_shapes.push_back( editableShape );
    }

    /// remove all shapes
    void
    reset()
    {
        m_shapes.resize( 0 );
    }

    /// get the current VGlist
    Carta::Lib::VectorGraphics::VGList
    vgList()
    {
        Carta::Lib::VectorGraphics::VGComposer comp;
        for ( auto & shape : m_shapes ) {
            if ( shape && shape-> isActive() ) {
                comp.appendList( shape-> getVGList() );
            }
        }
        return comp.vgList();
    }

    /// handle an input event
    void
    handleEvent( Carta::Lib::InputEvents::JsonEvent & ev );

    bool
    hasStateChanged() { return m_stateChanged; }

    virtual
    ~InteractiveShapesController() { }

    /// find shape under a point
    InteractiveShapeBase::SharedPtr
    findActiveShape( const QPointF & pt );

private:

    /// deliver double tap event
    bool
    interpretDoubleTapEvent( const Carta::Lib::InputEvents::DoubleTapEvent & ev );

    /// deliver hover event
    bool
    interpretHoverEvent( const Carta::Lib::InputEvents::HoverEvent & ev );

    /// deliver tap event
    bool
    interpretTouchEvent( const Carta::Lib::InputEvents::TouchEvent & ev );

    bool interpretDragEvent( const Carta::Lib::InputEvents::Drag2Event & ev);

    void
    dragStart( const QPointF & pt )
    {
        m_draggedShape = findActiveShape( pt );
        if ( m_draggedShape ) {
            m_draggedShape-> handleDragStart( pt );
        }
    }

    /// deliver drag event
    void
    dragProgress( const QPointF & pt )
    {
        if ( ! m_draggedShape ) {
            return;
        }
        m_draggedShape-> handleDrag( pt );
    }

    /// deliver drag done event
    void
    dragDone( const QPointF & pt )
    {
        if ( m_draggedShape ) {
            m_draggedShape-> handleDragDone( pt );
        }
        m_draggedShape = nullptr;
    }

private:

    InteractiveShapeBase::SharedPtr m_draggedShape = nullptr;
    bool m_stateChanged = true;

    std::vector < InteractiveShapeBase::SharedPtr > m_shapes;
};
}
