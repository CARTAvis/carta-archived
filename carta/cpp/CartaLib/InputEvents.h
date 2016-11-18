/**
 * Input events for carta
 **/

#pragma once

#include "CartaLib.h"
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QPointF>

namespace Carta
{
namespace Lib
{
namespace InputEvents
{
/// EventBase represents common functionality for all events
/// @note: please, no virtuals here, as we want to be able to pass events by value
class EventBase
{
public:

    /// returns the type of this event
    QString
    type() const;

    /// is this event valid?
    bool
    isValid() const;

    /// was this event consumed?
    bool
    isConsumed() const;

    /// set the consume flag to true
    void
    setConsumed( bool flag = true );

protected:

    /// we only allow derived classes to change the type
    void
    setType( QString type );

    /// we only allow derived classes to change validity
    void
    setValid( bool flag );

private:

    /// the only parsed portion of the json: type
    QString m_type;

    /// is this valid event
    bool m_valid = false;

    /// was this event consumed?
    bool m_consumed = false;
};

/// holder of a generic event described by a JSON
///
/// the json has to have a 'type' field that is a string, otherwise the event is declared
/// invalid
class JsonEvent
    : public EventBase
{
public:

    JsonEvent( const QJsonObject & json );

    /// return the raw json of this event
    const QJsonObject &
    json() const;

private:

    /// the entire json of the event
    QJsonObject m_json;
};

/// base for events with a single position (e.g. touch/tap/hover...)
/// @note: we are using this as a base class for the purposes of code reuse!
class PointerEvent
    : public EventBase
{
public:

    PointerEvent() {}

    PointerEvent( const JsonEvent & baseEvent) {
        init( baseEvent, QString());
    }

    /// \brief init
    /// \param baseEvent raw event from which to extract information
    /// \param eventType which type to match, unless Null, in which case no matching is done
    ///
    void
    init( const JsonEvent & baseEvent, QString eventType );

    const QPointF &
    pos() const;

    void
    setPos( const QPointF & pos );

private:

    QPointF m_pos;
};

/// event representing touch (e.g. mousedown, i.e. no mouse release necessary)
class TouchEvent
    : public PointerEvent
{
public:

    TouchEvent( const JsonEvent & baseEvent );
};

/// event representing a tap (e.g. click, i.e. release is necessary)
class TapEvent
    : public PointerEvent
{
public:

    TapEvent( const JsonEvent & baseEvent );
};

/// double tap event
class DoubleTapEvent
    : public PointerEvent
{
public:

    DoubleTapEvent( const JsonEvent & baseEvent );
};

/// hover event (e.g. mouse move)
class HoverEvent
    : public PointerEvent
{
public:

    HoverEvent( const JsonEvent & baseEvent );
};

/// event representing drag start
class DragStartEvent
    : public PointerEvent
{
public:

    DragStartEvent( const JsonEvent & baseEvent );
};

/// event for drag in progress
class DragEvent
    : public PointerEvent
{
public:

    DragEvent( const JsonEvent & baseEvent );
};

/// event for drag finished
class DragDoneEvent
    : public PointerEvent
{
public:

    DragDoneEvent( const JsonEvent & baseEvent );
};

class Drag2Event
    : public PointerEvent
{
public:

    enum class Phase { Start, End, Progress };

    Drag2Event( const JsonEvent & baseEvent );

    Phase
    phase() const;

private:

    Phase m_phase = Phase::End;
};
}
}
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
/*
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
*/
