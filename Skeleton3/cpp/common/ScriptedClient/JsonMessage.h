/**
 * Layer 3 : implemented on top of layer 2
 *
 * \note This is just a convenience layer...
 * I have not implemented JsonSocket because that would limit the communication to
 * JSON messages only. The JsonMessage can still be used relatively easily for communication
 * as it has the method to convert from/to TagMessage.
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "TagMessage.h"

#include <QJsonDocument>

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{
/// holds a Json message
/// can be serialized to/from TagMessage, with tag = "json"
class JsonMessage
{
public:

    explicit
    JsonMessage( const QJsonDocument & doc );

    TagMessage
    toTagMessage() const;

    /// will throw exception if message.tag != "json"
    static JsonMessage
    fromTagMessage( const TagMessage & message );

    const QJsonDocument &
    doc() const;

private:

    QJsonDocument m_doc;
    static constexpr char const * TAG = "json";
};
}
}
}
