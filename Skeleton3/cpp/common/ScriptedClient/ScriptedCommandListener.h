/**
 *
 **/

#pragma once

#include "ScriptFacade.h"
#include <QObject>

class QTcpServer;
class QTcpSocket;

class ScriptedCommandListener : public QObject
{
    Q_OBJECT
public:

    explicit ScriptedCommandListener(int port, QObject *parent = 0);
    bool sendTypedMessage( QString messageType, const void * data );

signals:

    void command( QString);

public slots:

private slots:

    void newConnectionCB();
    void socketDataCB();

private:

    bool sendNBytes( int n, const void * data );

    bool sendMessage( const void * data );

    bool receiveNBytes( int n, char** data );

    bool receiveMessage( char** data );

    int getMessageSize( );

    bool receiveTypedMessage( QString messageType, char** data );

    /// @todo make these unique_ptr<>
    QTcpServer * m_tcpServer = nullptr;
    QTcpSocket * m_connection = nullptr;
    ScriptFacade* m_scriptFacade = nullptr;
};

#ifdef DONT_COMPILE

#include "CartaLib/CartaLib.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <memory>

namespace Carta {
namespace Core {
namespace ScriptedClient {

/// Layer 1:
/// variable length message holding arbitrary binary data
typedef QByteArray VarLengthMessage;

/// Layer 2:
/// TagMessage is holds a string tag and binary data of variable length.
/// It can be serialized to/from VarLengthMessage.
class TagMessage {
public:
    TagMessage( QString tag, const VarLengthMessage & data)
        : m_tag(tag)
        , m_data( data)
    {;}

    /// tag getter
    const QString & tag() const { return m_tag; }

    /// raw data getter
    const QByteArray & data() const { return m_data; }

    /// serialize to variable length message
    VarLengthMessage toVarLengthMessage() const {
        VarLengthMessage buff;
        buff.append( tag());
        buff.append( '\0');
        buff.append( data());
        return buff;
    }

    /// deserialize from VarLengthMessage
    static TagMessage fromVarLengthMessage( const QByteArray & data) {
        QString tag = data.constData();
        VarLengthMessage ba = data.right( data.size() - tag.size() - 1);
        return TagMessage( tag, ba);
    }

private:

    QString m_tag;
    VarLengthMessage m_data;
};

/// holds a Json message
/// can be serialized to/from TagMessage, where tag = "json"
class JsonMessage
{

public:

    explicit JsonMessage( const QJsonDocument & doc) {
        m_doc = doc;
    }

    TagMessage toTagMessage() const {
        return TagMessage( "json", m_doc.toJson());
    }

    static JsonMessage fromTagMessage( const TagMessage & message) {
        CARTA_ASSERT( message.tag() == TAG);
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson( message.data(), & jsonError);
        if( doc.isEmpty() || jsonError.error != QJsonParseError::NoError) {
            qWarning() << "error in parsing received JSON";
            qWarning() << jsonError.errorString();
            qWarning() << "...in...";
            qWarning() << message.data();
            return JsonMessage( QJsonDocument());
        }
        return JsonMessage( doc);
    }

    const QJsonDocument & doc() const {
        return m_doc;
    }

private:

    QJsonDocument m_doc;
    static constexpr char * TAG = "json";
};

/// Wrapper around QTcpSocket, allowing us to send /receive variable length messages
/// The variable length messages are represented by QByteBuffer.
///
/// Messages are encoded:
/// 8 bytes representing the length of the data (n) in little endian
/// n bytes represnting the raw (binary) data
class VarLengthSocket : public QObject {
    Q_OBJECT
public:
    VarLengthSocket( std::shared_ptr<QTcpSocket> rawSocket)
        : QObject(nullptr)
        , m_rawSocket(rawSocket)
    {
        connect( m_rawSocket.get(), & QTcpSocket::readyRead,
                 this, & VarLengthSocket::socketCB);
    }
public slots:

    /// send the contents of data over the raw socket
    void send( const VarLengthMessage & data) {
        qint64 size = data.size();
        qint64 nsize = qToLittleEndian( size);
        sendNBytes( 8, & nsize);
        sendNBytes( size, data.constData());
    }

signals:

    /// emitted when a complete message arrives
    void received( VarLengthMessage data);

private slots:

    /// raw socket callback when data becomes available to be read
    void socketCB() {
        VarLengthMessage buff;
        qint64 size;
        readNBytes( 8, & size);
        size = qFromLittleEndian( size);
        buff.resize( size);
        readNBytes( size, buff.data());

        emit received( buff);
    }

private:

    /// Send raw bytes to a socket. It does not return untill all bytes were sent. If there
    /// was an error sending the data, an exception is thrown.
    /// \param n number of bytes
    /// \param data the raw data
    ///
    void sendNBytes( qint64 n, const void * data) {
        qint64 remaining = n;
        const char * ptr = (const char *) data;
        while( remaining > 0) {
            auto written = m_rawSocket-> write( ptr, remaining);
            if( written < 0) {
                throw std::runtime_error( "closed socket?");
            }
            remaining -= written;
            ptr += written;
        }
    }

    /// Reads n bytes from a socket and puts the result into memory pointed to by dest. This
    /// memory must be allocated by the caller. The method does not return until all n bytes
    /// have been read. If there is an error reading the socket, exception is thrown.
    /// \param n number of bytes to read
    /// \param dest where to store the data read
    ///
    void readNBytes( qint64 n, void * dest) {
        qint64 remaining = n;
        char * ptr = reinterpret_cast<char *>( dest );
        while( remaining > 0) {
            auto n = m_rawSocket-> read( ptr, remaining);
            if( n < 0) {
                throw std::runtime_error( "closed socket?");
            }
            remaining -= n;
            ptr += n;
        }
    }

    /// pointer to the actual raw socket
    std::shared_ptr<QTcpSocket> m_rawSocket = nullptr;
};

/// wrapper around QTcpSocket, with public api for sending / receiving TagMessage
/// Internally we use VarLengthSocket to do the work.
///
/// Message format:
/// - \0 terminated string for the tag
/// - followed by the raw binary data
class TagMessageSocket : public QObject {
    Q_OBJECT
public:
    TagMessageSocket( std::shared_ptr<QTcpSocket> rawSocket)
        : QObject(nullptr)
    {
        m_vlSocket.reset( new VarLengthSocket( rawSocket));
        connect( m_vlSocket.get(), & VarLengthSocket::received,
                 this, & TagMessageSocket::vlenMessageCB);
    }

    void send( const TagMessage & message) {
        // serialize and send the message
        m_vlSocket-> send( message.toVarLengthMessage());
    }

signals:

    /// emitted when a complete tag message arrives
    void received( TagMessage message);

private slots:

    /// internal callback invoked when VarLengthSocket receives a full message
    /// we decode it's raw data into tag and the rest of the data
    void vlenMessageCB( VarLengthMessage data) {
        emit received( TagMessage::fromVarLengthMessage( data));
    }

private:

    std::unique_ptr<VarLengthSocket> m_vlSocket = nullptr;
};

/// this class
///   - listens for incoming connection on a user specified port
///   - opens new connection
///   - any time a full TagMessage arrives, a signal is emitted
/// it can also be used to send TagMessage

class MessageListener : public QObject
{
    Q_OBJECT
public:

    explicit MessageListener(int port, QObject *parent = 0)
        : QObject(parent)
    {
        m_tcpServer.reset( new QTcpServer( this));

        // when new connection is established, call newConnectionCB()
        connect( m_tcpServer.get(), & QTcpServer::newConnection,
                 this, & MessageListener::newConnectionCB);

        // start listening for new connections on the port
        if( ! m_tcpServer-> listen( QHostAddress::AnyIPv4, port)) {
            throw std::runtime_error("Coud not listen for scripted commands on given port");
        }

    }
    bool send( const TagMessage & msg ) {
        try {
            m_tmSocket-> send( msg);
        }
        catch(...) {
            return false;
        }
        return true;
    }

    bool sendTypedMessage( QString messageType, const void * data ) {
        return send( TagMessage( messageType, QByteArray(reinterpret_cast<const char *>(data))));
    }

signals:

    void received( TagMessage message);

    /// keeping this for compatibility with old code
    void command( QString);

public slots:

private slots:

    void newConnectionCB() {
        if( m_tmSocket) {
            qWarning() << "Another client trying to connect? Ignoring...";
            auto socket = m_tcpServer->nextPendingConnection();
            socket->write( "Bye.");
            socket->waitForBytesWritten();
            delete socket;
            return;
        }
        auto sock = m_tcpServer->nextPendingConnection();
        m_tmSocket.reset( new TagMessageSocket( std::shared_ptr<QTcpSocket>(sock)));
        connect( m_tmSocket.get(), & TagMessageSocket::received,
                 this, & MessageListener::tagMessageReceivedCB);

    }

    void tagMessageReceivedCB( TagMessage msg) {
        emit received( msg);

        return;

        // translate the tag message into a 'command'
        if( msg.tag() != "1") {
            qWarning() << "Cannot handle messages with type" << msg.tag();
            return;
        }
        QString cmd = msg.data();
        emit command( cmd);
    }

private:
    /// @todo make these unique_ptr<>
    std::unique_ptr<QTcpServer> m_tcpServer = nullptr;
    std::unique_ptr<TagMessageSocket> m_tmSocket = nullptr;
    std::unique_ptr<ScriptFacade> m_scriptFacade = nullptr;
};


}
}
}

#endif
