#include "ScriptedCommandListener.h"
#include <QTcpServer>
#include <QTcpSocket>
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
    QString str( buffer );
    if (result == true) {
        str = str.trimmed();
        emit command( str);
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

int ScriptedCommandListener::getMessageSize( )
{
    // Instead of bool, this method will return an int.
    // A return value of 0 can be interpreted as an error.
    long long result;
    int messageSize = sizeof(long long);
    unsigned char size[messageSize+1];
    // This needs to be put into a loop in case the whole size cannot be read
    // all at once. Can I maybe just read one byte at a time?
    qint64 lineLength = 0;
    for (int i = 0; i < messageSize; i++) {
        char buff[2];
        int futileReads = 0;
        qint64 bytesRead = -1;
        while (bytesRead < 1) {
            bytesRead = m_connection-> readLine( buff, 2 );
            if (bytesRead == 0) {
                futileReads += 1;
            }
        }
        lineLength += bytesRead;
        size[i] = buff[0];
    }
    size[8] = NULL;
    if( lineLength == -1) {
        qWarning() << "scripted command listener: something wrong with socket";
        result = 0;
    }

    if ( lineLength < 8 ) {
        result = 0;
    }

    if (lineLength == 8) {
        result = (size[7]<<0) | (size[6]<<8) | (size[5]<<16) | (size[4]<<24) | (size[3]<<32) | (size[2]<<40) | (size[1]<<48) | (size[0]<<56);
    }
    return result;
}

bool ScriptedCommandListener::receiveMessage( QByteArray* data )
{
    //format: 4, 6, or 8 bytes: the size of the following message
    //after receiving this, enter a loop to receive this number of bytes
    bool result = 0;
    int size = getMessageSize();
    if ( size > 0 ) {
        result = receiveNBytes( size, data );
    }
    return result;
}

bool ScriptedCommandListener::receiveTypedMessage( QString messageType, QByteArray* data )
{
    if ( messageType == "1") {
        bool result = receiveMessage( data );
        return result;
    }
    else {
        return false;
    }
}
