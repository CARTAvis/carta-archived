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
}
