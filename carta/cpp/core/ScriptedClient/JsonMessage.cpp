/**
 *
 **/

#include "JsonMessage.h"

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{
JsonMessage::JsonMessage( const QJsonDocument & doc )
{
    m_doc = doc;
}

TagMessage
JsonMessage::toTagMessage() const
{
    return TagMessage( "json", m_doc.toJson() );
}

JsonMessage
JsonMessage::fromTagMessage( const TagMessage & message )
{
    CARTA_ASSERT( message.tag() == TAG || message.tag() == "async" );
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson( message.data(), & jsonError );
    if ( doc.isEmpty() || jsonError.error != QJsonParseError::NoError ) {
        qWarning() << "error in parsing received JSON";
        qWarning() << jsonError.errorString();
        qWarning() << "...in...";
        qWarning() << message.data();
        return JsonMessage( QJsonDocument() );
    }
    return JsonMessage( doc );
}

const QJsonDocument &
JsonMessage::doc() const
{
    return m_doc;
}
}
}
}
