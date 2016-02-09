/**
 *
 **/

#include "InteractiveShapes.h"


namespace editable
{

void InteractiveShapeBase::handleEvent(Carta::Lib::InputEvents::JsonEvent & ev)
{
    qDebug() << "Shape handling extra event" << ev.type();

}

void InteractiveShapesController::handleEvent(Carta::Lib::InputEvents::JsonEvent & ev)
{

    // handle hover event
    if ( interpretHoverEvent( Carta::Lib::InputEvents::HoverEvent( ev ) ) ) {
        return;
    }

    // handle touch event
    if ( interpretTouchEvent( Carta::Lib::InputEvents::TouchEvent( ev ) ) ) {
        return;
    }

    // handle double taps
    if ( interpretDoubleTapEvent( Carta::Lib::InputEvents::DoubleTapEvent( ev ) ) ) {
        return;
    }

    // handle drags
    if( interpretDragEvent( Carta::Lib::InputEvents::Drag2Event(ev))) {
        return;
    }

    qWarning() << "Dont understand event" << ev.type();

    // since we could not figure out what to do with this event,
    // try to deliver the event to shape

    // is this a pointer event?
    Carta::Lib::InputEvents::PointerEvent pev( ev );
    if ( ! pev.isValid() ) {
        // right now we only handle pointer events
        return;
    }

    // find the shape under the mouse
    InteractiveShapeBase::SharedPtr currShape = findActiveShape( pev.pos() );

    // if we did not find a shape under the mouse, we are done
    if ( ! currShape ) { return; }

    // deliver this event to the shape
    currShape-> handleEvent( ev );

    // mark the event consumed
    ev.setConsumed();
}

InteractiveShapeBase::SharedPtr InteractiveShapesController::findActiveShape(const QPointF & pt)
{
    InteractiveShapeBase::SharedPtr currShape = nullptr;

    //        for ( auto & shape : m_shapes ) {
    for ( auto i = m_shapes.rbegin() ; i != m_shapes.rend() ; ++i ) {
        auto & shape = * i;

        // skip deleted shapes
        if ( ! shape ) { continue; }

        // skip inactive shapes
        if ( ! shape-> isActive() ) { continue; }
        if ( shape-> isPointInside( pt ) ) {
            currShape = shape;
            break;
        }
    }
    return currShape;
}

bool InteractiveShapesController::interpretDoubleTapEvent(const Carta::Lib::InputEvents::DoubleTapEvent & ev)
{
    if ( ! ev.isValid() ) { return false; }

    const auto & pt = ev.pos();

    // find the shape under the mouse
    InteractiveShapeBase::SharedPtr currShape = findActiveShape( pt );

    bool change = false;

    // update edit flags
    for ( auto & shape : m_shapes ) {
        // skip deleted shapes
        if ( ! shape ) { continue; }

        // skip inactive shapes
        if ( ! shape-> isActive() ) { continue; }

        // if this is our shape, set its flag
        bool newFlag = shape == currShape;
        bool oldFlag = shape-> isInEditMode();
        if ( newFlag != oldFlag ) {
            shape-> setEditMode( newFlag );
            change = true;
        }
    }

    m_stateChanged = change;
    return true;
}

bool InteractiveShapesController::interpretHoverEvent(const Carta::Lib::InputEvents::HoverEvent & ev)
{
    if ( ! ev.isValid() ) { return false; }
    const auto & pt = ev.pos();

    // find the shape under the mouse
    InteractiveShapeBase::SharedPtr currShape = findActiveShape( pt );

    bool change = false;

    // reset all hovered flags
    for ( auto & shape : m_shapes ) {
        // skip deleted shapes
        if ( ! shape ) { continue; }

        // skip inactive shapes
        if ( ! shape-> isActive() ) { continue; }

        // if this is our shape, set its flag
        bool newFlag = shape == currShape;
        if ( shape-> setIsHovered( newFlag ) ) {
            change = true;
        }
    }

    m_stateChanged = change;
    return true;
}

bool InteractiveShapesController::interpretTouchEvent(const Carta::Lib::InputEvents::TouchEvent & ev)
{
    if ( ! ev.isValid() ) { return false; }
    const auto & pt = ev.pos();

    // find the shape under the mouse
    InteractiveShapeBase::SharedPtr currShape = findActiveShape( pt );

    bool change = false;

    // reset all hovered flags
    for ( auto & shape : m_shapes ) {
        // skip deleted shapes
        if ( ! shape ) { continue; }

        // skip inactive shapes
        if ( ! shape-> isActive() ) { continue; }

        // if this is our shape, set its flag
        bool newFlag = shape == currShape;
        bool oldFlag = shape-> getIsSelected();
        if ( newFlag != oldFlag ) {
            shape-> setIsSelected( newFlag );
            change = true;
        }
    }

    m_stateChanged = change;
    return true;
}

bool InteractiveShapesController::interpretDragEvent(const Carta::Lib::InputEvents::Drag2Event & ev)
{
    if ( ! ev.isValid() ) { return false; }
    const auto & pt = ev.pos();
    auto phase = ev.phase();

    switch (phase) {
    case Carta::Lib::InputEvents::Drag2Event::Phase::Start:
        dragStart( pt);
        break;
    case Carta::Lib::InputEvents::Drag2Event::Phase::End:
        dragDone( pt);
        break;
    case Carta::Lib::InputEvents::Drag2Event::Phase::Progress:
        dragProgress( pt);
        break;
    default:
        break;
    }
    return true;

}

}
