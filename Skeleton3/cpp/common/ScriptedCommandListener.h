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

    QString dataTransporter(QString input);

    bool receiveNBytes( int n, QString& data );

    bool receiveMessage( QString& data);

    bool receiveTypedMessage( QString messageType, QString& data );

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

