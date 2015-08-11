/**
 * Layer 1 : Implemented on top of layer 0 (raw QTcpSocket)
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include <QByteArray>
#include <QTcpSocket>
#include <QtEndian>

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{
/// Layer 1:
/// variable length message holding arbitrary binary data
typedef QByteArray VarLengthMessage;

/// Wrapper around QTcpSocket, allowing us to send /receive variable length messages
/// The variable length messages are represented by QByteBuffer.
///
/// Messages are encoded:
/// 8 bytes representing the length of the data (n) in little endian
/// n bytes represnting the raw (binary) data
class VarLengthSocket : public QObject
{
    Q_OBJECT

public:

    VarLengthSocket( std::shared_ptr < QTcpSocket > rawSocket );

public slots:

    /// send the contents of data over the raw socket
    void
    send( const VarLengthMessage & data );

signals:

    /// emitted when a complete message arrives
    void
    received( VarLengthMessage data );

private slots:

    /// raw socket callback when data becomes available to be read
    void
    socketCB();

private:

    /// Send raw bytes to a socket. It does not return untill all bytes were sent. If there
    /// was an error sending the data, an exception is thrown.
    /// \param n number of bytes
    /// \param data the raw data
    ///
    void
    sendNBytes( qint64 n, const void * data );

    /// Reads n bytes from a socket and puts the result into memory pointed to by dest. This
    /// memory must be allocated by the caller. The method does not return until all n bytes
    /// have been read. If there is an error reading the socket, exception is thrown.
    /// \param n number of bytes to read
    /// \param dest where to store the data read
    ///
    void
    readNBytes( qint64 n, void * dest );

    /// pointer to the actual raw socket
    std::shared_ptr < QTcpSocket > m_rawSocket = nullptr;
};
}
}
}
