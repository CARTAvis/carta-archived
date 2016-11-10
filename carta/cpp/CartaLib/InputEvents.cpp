/**
 *
 **/

#include "InputEvents.h"

namespace Carta
{
namespace Lib
{
namespace InputEvents
{
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
    JsonEvent be( jobj );

    TouchEvent te( be );

//    std::unique_ptr < TouchEvent > te( convertInputEvent < TouchEvent > ( be ) );
    if ( te.isValid() ) {
        qDebug() << "Touch event good" << te.pos();
    }
    else {
        qDebug() << "Touch event no good";
    }
    return 1;
} // eventApiTest

//static auto xxx = eventApiTest();

QString
EventBase::type() const
{
    return m_type;
}

bool
EventBase::isValid() const
{
    return m_valid;
}

bool
EventBase::isConsumed() const
{
    return m_consumed;
}

void
EventBase::setConsumed( bool flag )
{
    if ( CARTA_RUNTIME_CHECKS ) {
        if ( m_consumed && flag ) {
            qWarning() << "Double consume of event?" << type();
        }
    }
    m_consumed = flag;
}

void
EventBase::setType( QString type )
{
    m_type = type;
}

void
EventBase::setValid( bool flag )
{
    m_valid = flag;
}

JsonEvent::JsonEvent( const QJsonObject & json )
{
    // save the raw json
    m_json = json;

    // find the type field
    auto it = m_json.find( "type" );
    if ( it == m_json.end() || ! it.value().isString() ) {
        // quit now, leaving the event in invalid state
        return;
    }
    setType( it.value().toString() );
    setValid( ! type().isNull() );
}

const QJsonObject &
JsonEvent::json() const
{
    return m_json;
}

void
PointerEvent::init( const JsonEvent & baseEvent, QString eventType )
{
    // match eventType if given
    if ( ! eventType.isNull() && baseEvent.type() != eventType ) {
        return;
    }

    // set the same type as base event
    setType( baseEvent.type() );

    // parse x,y position
    if ( ! baseEvent.json()["x"].isDouble() ) {
        return;
    }
    if ( ! baseEvent.json()["y"].isDouble() ) {
        return;
    }
    m_pos.rx() = baseEvent.json()["x"].toDouble();
    m_pos.ry() = baseEvent.json()["y"].toDouble();

    // validate the event
    setValid( true );
} // init

const QPointF &
PointerEvent::pos() const
{
    return m_pos;
}

void
PointerEvent::setPos( const QPointF & pos )
{
    m_pos = pos;
}

TouchEvent::TouchEvent( const JsonEvent & baseEvent )
{
    init( baseEvent, "touch" );
}

TapEvent::TapEvent( const JsonEvent & baseEvent )
{
    init( baseEvent, "tap" );
}

DoubleTapEvent::DoubleTapEvent( const JsonEvent & baseEvent )
{
    init( baseEvent, "dbltap" );
}

HoverEvent::HoverEvent( const JsonEvent & baseEvent )
{
    init( baseEvent, "hover" );
}

DragStartEvent::DragStartEvent( const JsonEvent & baseEvent )
{
    init( baseEvent, "dragstart" );
}

DragEvent::DragEvent( const JsonEvent & baseEvent )
{
    init( baseEvent, "drag" );
}

DragDoneEvent::DragDoneEvent( const JsonEvent & baseEvent )
{
    init( baseEvent, "dragdone" );
}

Drag2Event::Drag2Event( const JsonEvent & baseEvent )
{
    init( baseEvent, "drag2" );

    QString phase = baseEvent.json()["phase"].toString();
    if ( phase == "start" ) {
        m_phase = Phase::Start;
    }
    else if ( phase == "end" ) {
        m_phase = Phase::End;
    }
    else if ( phase == "progress" ) {
        m_phase = Phase::Progress;
    }
    else {
        setValid( false );
    }
}

Drag2Event::Phase
Drag2Event::phase() const
{
    return m_phase;
}
}
}
}
