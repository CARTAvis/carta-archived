/**
 *
 **/

#pragma once

#include "ScriptFacade.h"
#include "TagMessage.h"
#include <QObject>

class QTcpServer;
class QTcpSocket;

class ScriptedCommandListener : public QObject
{
    Q_OBJECT
public:

    explicit ScriptedCommandListener(int port, QObject *parent = 0);
    bool sendTypedMessage( QString messageType, const void * data );

signals:

    void command( QString);

public slots:

private slots:

    void newConnectionCB();
    void socketDataCB();

private:

    bool sendNBytes( int n, const void * data );

    bool sendMessage( const void * data );

    bool receiveNBytes( int n, QByteArray* data );

    void readNBytes( qint64 n, void * dest );

    bool receiveMessage( QByteArray* data );

    bool receiveTypedMessage( QString messageType, QByteArray* data );

    /// @todo make these unique_ptr<>
    QTcpServer * m_tcpServer = nullptr;
    QTcpSocket * m_connection = nullptr;
    ScriptFacade* m_scriptFacade = nullptr;
};

