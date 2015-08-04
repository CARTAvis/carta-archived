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

signals:

    void command( QString);

public slots:

private slots:

    void newConnectionCB();

private:

    bool receiveNBytes( int n, QByteArray* data );

    void readNBytes( qint64 n, void * dest );

    bool receiveMessage( QByteArray* data );

    bool receiveTypedMessage( QString messageType, QByteArray* data );

    /// @todo make these unique_ptr<>
    QTcpServer * m_tcpServer = nullptr;
    QTcpSocket * m_connection = nullptr;
    ScriptFacade* m_scriptFacade = nullptr;
};

