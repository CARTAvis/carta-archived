#include "ScriptedCommandListener.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <stdexcept>
#include <netinet/in.h>
#include <iostream>

const QString ScriptedCommandListener::SIZE_DELIMITER(":");

ScriptedCommandListener::ScriptedCommandListener(int port, QObject * parent)
    : QObject(parent)
{
    m_tcpServer = new QTcpServer( this);

    /// make sure we know when new connection is established
    connect( m_tcpServer, & QTcpServer::newConnection,
             this, & ScriptedCommandListener::newConnectionCB);

    /// now listen to the port
    if( ! m_tcpServer-> listen( QHostAddress::AnyIPv4, port)) {
        throw std::runtime_error("Coud not listen for scripted commands on given port");
    }
}

void ScriptedCommandListener::newConnectionCB()
{
    qDebug() << "New scripted client connection...";
    if( m_connection != nullptr) {
        qWarning() << "Another client trying to connect? Ignoring...";
        return;
    }
    m_connection = m_tcpServer->nextPendingConnection();
    connect( m_connection, & QTcpSocket::readyRead,
             this, & ScriptedCommandListener::socketDataCB);

}

void ScriptedCommandListener::socketDataCB()
{
    char* buffer;
    bool result = receiveTypedMessage( "1", &buffer );
    QString str = QString::fromUtf8(buffer);
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
    bool result;
    int bytesSent = 0;
    while (bytesSent < n) {
        int sent = m_connection->write( ptr, n-bytesSent );
        if (sent < 0) {
            result = false;
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

bool ScriptedCommandListener::sendTypedMessage( QString messageType, const void * data )
{
    bool result = sendMessage( data );
    return result;
}

//bool ScriptedCommandListener::receiveNBytes( int n, QString& data )
bool ScriptedCommandListener::receiveNBytes( int n, char** data )
{
    qDebug() << "receiveNBytes";
    bool result = true;
    char* buff = new char[n+1];
    qDebug() << "buff created";
    int buffIndex = 0;
    qint64 lineLength = 0;
    while (lineLength < n) {
        //char tempBuff[n];
        char* tempBuff = new char[n+1];
        qint64 bytesRead = -1;
        int futileReads = 0;
        while (bytesRead < 1) {
            bytesRead = m_connection-> readLine( tempBuff, n+1 );
            if (bytesRead == 0) {
                futileReads += 1;
            }
        }
        // Now copy what was read into buff[]
        for (int i = 0; i < bytesRead && buffIndex < n; i++) {
            buff[buffIndex] = tempBuff[i];
            buffIndex++;
        }
        lineLength += bytesRead;
    }
    // Why should this be necessary? I sometimes get garbage data without it.
    buff[n] = NULL;
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

    if (result == true) {
        *data = (char *) malloc( strlen(buff) + 1 ); 
        strcpy( *data, buff );
    }
    return result;
}

int ScriptedCommandListener::getMessageSize( )
{
    // Instead of bool, this method will return an int.
    // A return value of 0 can be interpreted as an error.
    int result;
    int messageSize = sizeof(int);
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
        //int buffInt = int(*(unsigned char*)buff);
        //std::cout << "buffInt " << std::hex << buffInt << "\n";
        lineLength += bytesRead;
        size[i] = buff[0];
    }
    size[4] = NULL;
    if( lineLength == -1) {
        qWarning() << "scripted command listener: something wrong with socket";
        result = 0;
    }

    if ( lineLength < 4 ) {
        result = 0;
    }

    if (lineLength == 4) {
        result = (size[3]<<0) | (size[2]<<8) | (size[1]<<16) | (size[0]<<24);
    }
    return result;
}

bool ScriptedCommandListener::receiveMessage( char** data )
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

bool ScriptedCommandListener::receiveTypedMessage( QString messageType, char** data )
{
    bool result = receiveMessage( data );
    return result;
}
