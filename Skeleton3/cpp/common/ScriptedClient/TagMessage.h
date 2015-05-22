/**
 * Layer 2 : Implemented on top of layer 1
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "VarLengthMessage.h"

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{
/// Layer 2:
/// TagMessage is holds a string tag and binary data of variable length.
/// It can be serialized to/from VarLengthMessage.
class TagMessage
{
public:

    TagMessage( QString tag, const VarLengthMessage & data );

    /// tag getter
    const QString &
    tag() const;

    /// raw data getter
    const QByteArray &
    data() const { return m_data; }

    /// serialize to variable length message
    VarLengthMessage
    toVarLengthMessage() const;

    /// deserialize from VarLengthMessage
    static TagMessage
    fromVarLengthMessage( const QByteArray & data );

private:

    QString m_tag;
    VarLengthMessage m_data;
};

/// wrapper around QTcpSocket, with public api for sending / receiving TagMessage
/// Internally we use VarLengthSocket to do the work.
///
/// Message format:
/// - \0 terminated string for the tag
/// - followed by the raw binary data
class TagMessageSocket : public QObject
{
    Q_OBJECT

public:

    TagMessageSocket( std::shared_ptr < QTcpSocket > rawSocket );

    void
    send( const TagMessage & message );

signals:

    /// emitted when a complete tag message arrives
    void
    received( TagMessage message );

private slots:

    /// internal callback invoked when VarLengthSocket receives a full message
    /// we decode it's raw data into tag and the rest of the data
    void
    vlenMessageCB( VarLengthMessage data );

private:

    std::unique_ptr < VarLengthSocket > m_vlSocket = nullptr;
};
}
}
}
