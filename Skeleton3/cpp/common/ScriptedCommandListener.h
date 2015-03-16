/**
 *
 **/

#pragma once

#include <QObject>
#include "ScriptFacade.h"

class QTcpServer;
class QTcpSocket;

class ScriptedCommandListener : public QObject
{
    Q_OBJECT
public:
    explicit ScriptedCommandListener(int port, QObject *parent = 0);

    bool sendNBytes( int n, const void * data );

    bool sendMessage( const void * data );

    bool sendTypedMessage( QString messageType, const void * data );

    bool receiveNBytes( int n, QByteArray* data );

    bool receiveMessage( QByteArray* data );

    int getMessageSize( );

    bool receiveTypedMessage( QString messageType, QByteArray* data );

    static const QString SIZE_DELIMITER;

signals:

    void command( QString);

public slots:

protected slots:

    void newConnectionCB();
    void socketDataCB();

protected:

    QTcpServer * m_tcpServer = nullptr;
    QTcpSocket * m_connection = nullptr;
    ScriptFacade* m_scriptFacade;
};

