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
    qDebug() << "scripted command listener: socket data ready";
    char* buffer;
    bool result = receiveTypedMessage( "whatever", &buffer );
    QString str = QString::fromUtf8(buffer);
    qDebug() << "(JT) ScriptedCommandListener::socketDataCB str = " << str;
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
    //data = QString::number(n) + ScriptedCommandListener::SIZE_DELIMITER + data;
    const char * ptr = reinterpret_cast<const char *> ( data);
    qDebug() << "sendNBytes n = "<< n;
    qDebug() << "sendNBytes ptr = "<< ptr;
    //std::cout << "Sending " << std::hex << int( * (char *)(ptr)) << "\n";
    bool result;
    int bytesSent = 0;
    while (bytesSent < n) {
        int sent = m_connection->write( ptr, n-bytesSent );
        qDebug() << "(JT) sendNBytes sent this time =" << sent;
        if (sent < 0) {
            result = false;
            break;
        }
        bytesSent += sent;
        qDebug() << "(JT) sendNBytes bytesSent =" << bytesSent;
        ptr += sent;
        //qDebug() << "sendNBytes ptr in the loop = "<< ptr;
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
    qDebug() << "(JT) sendTypedMessage() messageType =" << messageType;
    bool result = sendMessage( data );
    return result;
}

//bool ScriptedCommandListener::receiveNBytes( int n, QString& data )
bool ScriptedCommandListener::receiveNBytes( int n, char** data )
{
    bool result = true;
    char buff[n];
    int buffIndex = 0;
    qint64 lineLength = 0;
    while (lineLength < n) {
        char tempBuff[n];
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
        qDebug() << "(JT) receiveNBytes() made " << futileReads << "futile attempts";
    }
    // Why should this be necessary? I sometimes get garbage data without it.
    buff[n] = NULL;
    if( lineLength == -1) {
        qWarning() << "scripted command listener: something wrong with socket";
        result = false;
    }

    if( lineLength == 0) {
        qDebug() << "scripted command listener: not a full line yet...";
        result = false;
    }

    if ( lineLength < n ) {
        qDebug() << "(JT) receiveNBytes() haven't received the full message yet.";
        result = false;
    }

    if ( lineLength > n ) {
        qDebug() << "(JT) receiveNBytes() received too much data.";
        result = false;
    }

    //qDebug() << "(JT) receiveNBytes() data before = " << *data;
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
    char message[messageSize];
    // This needs to be put into a loop in case the whole size cannot be read
    // all at once. Can I maybe just read one byte at a time?
    qint64 lineLength = 0;
    for (int i = 0; i < messageSize; i++) {
        char buff[1];
        int futileReads = 0;
        qint64 bytesRead = -1;
        while (bytesRead < 1) {
            bytesRead = m_connection-> readLine( buff, 2 );
            if (bytesRead == 0) {
                futileReads += 1;
            }
        }
        qDebug() << "(JT) getMessageSize() made " << futileReads << "futile attempts for byte " << i;
        lineLength += bytesRead;
        message[i] = buff[0];
    }
    if( lineLength == -1) {
        qWarning() << "scripted command listener: something wrong with socket";
        result = -1;
    }

    if( lineLength == 0) {
        qDebug() << "scripted command listener: not a full line yet...";
        result = -1;
    }

    if ( lineLength < 4 ) {
        qDebug() << "(JT) getMessageSize() haven't received the full size yet.";
    }

    if ( lineLength > 4 ) {
        qDebug() << "(JT) getMessageSize() received too much data.";
    }

    if (lineLength >= 1) {
        result = (message[3]<<0) | (message[2]<<8) | (message[1]<<16) | (message[0]<<24);
    }
    return result;
}

bool ScriptedCommandListener::receiveMessage( char** data )
{
    //format: 4, 6, or 8 bytes: the size of the following message
    //after receiving this, enter a loop to receive this number of bytes
    int size = getMessageSize();
    bool result = receiveNBytes( size, data );
    return result;
}

bool ScriptedCommandListener::receiveTypedMessage( QString messageType, char** data )
{
    qDebug() << "(JT) receiveTypedMessage() messageType =" << messageType;
    bool result = receiveMessage( data );
    return result;
}
