#include "ScriptedCommandListener.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QtEndian>
#include <QDebug>
#include <stdexcept>
#include <netinet/in.h>
#include <iostream>

ScriptedCommandListener::ScriptedCommandListener(int port, QObject * parent)
    : QObject(parent)
{
    m_tcpServer = new QTcpServer( this);

    // when new connection is established, call newConnectionCB()
    connect( m_tcpServer, & QTcpServer::newConnection,
             this, & ScriptedCommandListener::newConnectionCB);

    // start listening for new connections on the port
    if( ! m_tcpServer-> listen( QHostAddress::AnyIPv4, port)) {
        throw std::runtime_error("Coud not listen for scripted commands on given port");
    }
}

void ScriptedCommandListener::newConnectionCB()
{
    qDebug() << "New scripted client connection...";
    if( m_connection != nullptr) {
        qWarning() << "Another client trying to connect? Ignoring & closing.";
        auto socket = m_tcpServer->nextPendingConnection();
        socket->write( "Bye." );
        socket->waitForBytesWritten();
        delete socket;
        return;
    }
    m_connection = m_tcpServer->nextPendingConnection();
    connect( m_connection, & QTcpSocket::readyRead,
             this, & ScriptedCommandListener::socketDataCB);

}

void ScriptedCommandListener::socketDataCB()
{
    QByteArray buffer;
    bool result = receiveTypedMessage( "1", &buffer );
    qDebug() << "(JT) socketDataCB() &buffer =" << &buffer;
    qDebug() << "(JT) socketDataCB() buffer.data() =" << buffer.data();
    QString tag = buffer.constData();
    qDebug() << "(JT) socketDataCB tag =" << tag;
    QByteArray ba = buffer.right( buffer.size() - tag.size() - 1 );
    qDebug() << "(JT) socketDataCB() ba =" << ba;
    //QString str( buffer );
    if (result == true) {
        //str = str.trimmed();
        //emit command( str);
        emit command( ba.data() );
        qDebug() << "(JT) Whatever...";
    }
    else {
        qDebug() << "something went wrong in ScriptedCommandListener::socketDataCB()";
    }
}

bool ScriptedCommandListener::sendNBytes( int n, const void * data )
{
    const char * ptr = reinterpret_cast<const char *> ( data);
    int bytesSent = 0;
    while (bytesSent < n) {
        int sent = m_connection->write( ptr, n-bytesSent );
        if (sent < 0) {
            break;
        }
        bytesSent += sent;
        ptr += sent;
    }
    return true;
}

bool ScriptedCommandListener::sendMessage( const void * data )
{
    // format: 4, 6, or 8 bytes: the size of the following message
    char * dataStr = (char *) data;
    uint32_t dataLength = strlen(dataStr);
    // Convert the length to network order
    uint32_t dataLengthNet = htonl(dataLength);
    bool result = sendNBytes( 4, &dataLengthNet );
    result = sendNBytes( dataLength, data );
    return result;
}

bool ScriptedCommandListener::sendTypedMessage( QString /*messageType*/, const void * data )
{
    bool result = sendMessage( data );
    return result;
}

bool ScriptedCommandListener::receiveNBytes( int n, QByteArray* data )
{
    bool result = true;
    int buffIndex = 0;
    qint64 lineLength = 0;
    while (lineLength < n) {
        QByteArray tempBuff;
        qint64 bytesRead = -1;
        while (bytesRead < 1) {
            tempBuff = m_connection->readLine( n );
            bytesRead = tempBuff.size();
        }
        data->append( tempBuff );
        lineLength += bytesRead;
    }
    if( lineLength == -1) {
        qWarning() << "scripted command listener: something wrong with socket";
        result = false;
    }

    if( lineLength == 0) {
        result = false;
    }

    if ( lineLength < n ) {
        result = false;
    }

    if ( lineLength > n ) {
        result = false;
    }

    return result;
}

void ScriptedCommandListener::readNBytes( qint64 n, void * dest )
{
    qint64 remaining = n;
    char * ptr = reinterpret_cast < char * > ( dest );
    while ( remaining > 0 ) {
        auto n = m_connection->read( ptr, remaining );
        if ( n < 0 ) {
            throw std::runtime_error( "closed socket?" );
        }
        remaining -= n;
        ptr += n;
    }
}

bool ScriptedCommandListener::receiveMessage( QByteArray* data )
{
    //format: 4, 6, or 8 bytes: the size of the following message
    //after receiving this, enter a loop to receive this number of bytes
    bool result = true;
    qint64 size;
    readNBytes( 8, &size );
    size = qFromLittleEndian( size );
    data->resize( size );
    qDebug() << "(JT) receiveMessage() size =" << size;
    if ( size > 0 ) {
        readNBytes( size, data->data() );
    }
    qDebug() << "(JT) receiveMessage() data =" << data;
    qDebug() << "(JT) receiveMessage() data->data() =" << data->data();
    return result;
}

bool ScriptedCommandListener::receiveTypedMessage( QString messageType, QByteArray* data )
{
    if ( messageType == "1") {
        qDebug() << "(JT) receiveTypedMessage() data (before) =" << data;
        qDebug() << "(JT) receiveTypedMessage() data->data() (before) =" << data->data();
        bool result = receiveMessage( data );
        qDebug() << "(JT) receiveTypedMessage() data =" << data;
        qDebug() << "(JT) receiveTypedMessage() data->data() =" << data->data();
        return result;
    }
    else {
        return false;
    }
}
