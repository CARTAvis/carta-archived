#include "ScriptedCommandListener.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <stdexcept>

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
    //QString str;
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

QString ScriptedCommandListener::dataTransporter( QString input )
{
    int inputSize = input.size();
    // Prepend the data with "size:"
    // Python can then partition the incoming data to determine how much data
    // it should actually be receiving and make a second attempt to get the
    // rest, if necessary.
    input = QString::number(inputSize) + ScriptedCommandListener::SIZE_DELIMITER + input;
    m_connection->write( input.toLocal8Bit() );
    return input;
}

//bool ScriptedCommandListener::receiveNBytes( int n, QString& data )
bool ScriptedCommandListener::receiveNBytes( int n, char** data )
{
    bool result = true;
    char buff[n];
    qint64 lineLength = m_connection-> readLine( buff, n+1 );
    qDebug() << "(JT) receiveNBytes() lineLength = " << lineLength;
    if( lineLength == -1) {
        qWarning() << "scripted command listener: something wrong with socket";
        result = false;
    }

    if( lineLength == 0) {
        qDebug() << "scripted command listener: not a full line yet...";
        result = false;
    }

    qDebug() << "(JT) receiveNBytes() data before = " << data;
    if (result == true) {
        *data = buff;
    }
    qDebug() << "(JT) receiveNBytes() data after = " << data;
    return result;
}

//bool ScriptedCommandListener::receiveMessage( QString& data )
bool ScriptedCommandListener::receiveMessage( char** data )
{
    //format: 4, 6, or 8 bytes: the size of the following message
    //after receiving this, enter a loop to receive this number of bytes
    //QString sizeStr;
    int sizeLength = sizeof(int);
    qDebug() << "(JT) receiveMessage() sizeLength = " << sizeLength;
    char* sizeStr[sizeLength];
    bool result = receiveNBytes(sizeLength, sizeStr);
    //unsigned long size = sizeStr.toULong();
    int size = atoi( *sizeStr );
    qDebug() << "(JT) receiveMessage() size = " << size;
    qDebug() << "(JT) receiveMessage() data before = " << data;
    result = receiveNBytes( 1000000, data );
    qDebug() << "(JT) receiveMessage() data after = " << data;
    return result;
}

bool ScriptedCommandListener::receiveTypedMessage( QString messageType, char** data )
{
    qDebug() << "(JT) receiveTypedMessage() data before = " << data;
    bool result = receiveMessage( data );
    qDebug() << "(JT) receiveTypedMessage() data after = " << data;
    return result;
}
