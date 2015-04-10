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

}
}
}
