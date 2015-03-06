#include "ScriptedCommandListener.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <stdexcept>
#include <netinet/in.h>

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

bool ScriptedCommandListener::sendNBytes( int n, QString data )
{
    //data = QString::number(n) + ScriptedCommandListener::SIZE_DELIMITER + data;
    bool result;
    int bytesSent = 0;
    while (bytesSent < n) {
        bytesSent += m_connection->write( data.toLocal8Bit() );
        if (bytesSent < 0) {
            result = false;
            break;
        }
        data = data.right( n - bytesSent );
    }
    return true;
}

bool ScriptedCommandListener::sendMessage( QString data )
{
    // format: 4, 6, or 8 bytes: the size of the following message
    int dataLength = data.length();
    // For now, just send the length as an 8 character string, padded with
    // zeros if necessary. This can be changed to a binary representation
    // later.
    QString lengthStr = QString("%1").arg(dataLength, 8, 10, QChar('0'));
    int totalMessageLength = (data + lengthStr).length();
    qDebug() << "(JT) sendMessage() what's being sent: " << lengthStr + data;
    qDebug() << "(JT) sendMessage() totalMessageLength =" << totalMessageLength;
    bool result = sendNBytes( totalMessageLength, lengthStr + data );
    //m_connection->write(lengthStr.toLocal8Bit());
    //m_connection->write(data.toLocal8Bit());
    return result;
}

bool ScriptedCommandListener::sendTypedMessage( QString messageType, QString data )
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
