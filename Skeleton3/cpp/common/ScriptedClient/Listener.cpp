/**
 *
 **/

#include "Listener.h"

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{
MessageListener::MessageListener( int port, QObject * parent )
    : QObject( parent )
{
    m_tcpServer.reset( new QTcpServer( this ) );

    // when new connection is established, call newConnectionCB()
    connect( m_tcpServer.get(), & QTcpServer::newConnection,
             this, & MessageListener::newConnectionCB );

    // start listening for new connections on the port
    if ( ! m_tcpServer-> listen( QHostAddress::AnyIPv4, port ) ) {
        throw std::runtime_error( "Coud not listen for scripted commands on given port" );
    }

    qDebug() << "MessageListener listening on port" << port;
}

bool
MessageListener::send( const TagMessage & msg )
{
    try {
        m_tmSocket-> send( msg );
    }
    catch ( ... ) {
        return false;
    }
    return true;
}

bool
MessageListener::sendTypedMessage( QString messageType, const void * data )
{
    return send( TagMessage( messageType, QByteArray( reinterpret_cast < const char * > ( data ) ) ) );
}

void
MessageListener::newConnectionCB()
{
    if ( m_tmSocket ) {
        qWarning() << "Another client trying to connect? Old connection will be aborted...";

        // alternatively, enable below if you instead want to disable
        // additional connections...
        // For easier debugging I allow re-connection.
        if ( 0 ) {
            qWarning() << "Another client trying to connect? Refusing...";
            auto socket = m_tcpServer->nextPendingConnection();
            socket->write( "Bye." );
            socket->waitForBytesWritten();
            delete socket;
            return;
        }
    }
    QTcpSocket * sock = m_tcpServer->nextPendingConnection();
    m_tmSocket.reset( new TagMessageSocket( std::shared_ptr < QTcpSocket > ( sock ) ) );
    connect( m_tmSocket.get(), & TagMessageSocket::received,
             this, & MessageListener::tagMessageReceivedCB );
} // newConnectionCB

void
MessageListener::tagMessageReceivedCB( TagMessage msg )
{
    /// we just re-emit the message as is
    emit received( msg );
}

PavolCommandController::PavolCommandController( int port, QObject * parent )
    : QObject( parent )
{
    qDebug() << "PavolCommandController starting on port:" << port;

    m_messageListener.reset( new MessageListener( port, this ) );

    connect( m_messageListener.get(), & MessageListener::received,
             this, & PavolCommandController::tagMessageReceivedCB );
}

/// this is just a quick demo how to listen to TagMessage, convert them to Json,
/// extract info from Json, and pack results back into Json, then to TagMessage...
///
/// it could probably use some error checking, like asserts etc...
void
PavolCommandController::tagMessageReceivedCB( TagMessage tm )
{
    if ( tm.tag() != "json" ) {
        qWarning() << "I don't handle tag" << tm.tag();
        return;
    }
    JsonMessage jm = JsonMessage::fromTagMessage( tm );
    qDebug() << "json message=" << jm.doc().toJson();
    if ( ! jm.doc().isObject() ) {
        qWarning() << "Received json is not object...";
        return;
    }
    QJsonObject jo = jm.doc().object();
    QString cmd = jo["cmd"].toString();
    if ( cmd == "ls" ) {
        auto args = jo["args"].toObject();
        QString dir = args["dir"].toString();
        if ( dir.isEmpty() ) {
            dir = "/";
        }
        QStringList list = QDir( dir ).entryList();
        QJsonObject rjo;
        rjo.insert( "result", QJsonArray::fromStringList( list ) );
        JsonMessage rjm = JsonMessage( QJsonDocument( rjo ) );
        m_messageListener->send( rjm.toTagMessage() );
    }
    else if ( cmd == "add" ) {
        auto args = jo["args"].toObject();
        double a = args["a"].toDouble();
        double b = args["b"].toDouble();
        QJsonObject rjo;
        rjo.insert( "result", a + b );
        JsonMessage rjm = JsonMessage( QJsonDocument( rjo ) );
        m_messageListener->send( rjm.toTagMessage() );
    }
    else {
        qDebug() << "Unknown command, sending error back";
        m_messageListener->send( TagMessage( "json", "{ 'error':'Uknown command'}" ) );
    }
} // tagMessageReceivedCB
}
}
}
