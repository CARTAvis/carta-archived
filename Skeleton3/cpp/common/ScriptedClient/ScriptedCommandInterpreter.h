/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "ScriptedClient/ScriptFacade.h"
#include "Listener.h"
#include "TagMessage.h"
#include "JsonMessage.h"
#include <QTcpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <memory>

namespace Carta
{
namespace Core
{
namespace ScriptedClient
{
/// listens for some json commands, interprets them and sends results back
class ScriptedCommandInterpreter : public QObject
{
    Q_OBJECT

public:

    ScriptedCommandInterpreter( int port, QObject * parent = nullptr );

protected:

    ScriptFacade* m_scriptFacade = nullptr;

private slots:

    /// interpret commands, send results back
    void
    tagMessageReceivedCB( TagMessage tm );

    /// interpret commands with asynchronous results
    void
    asyncMessageReceivedCB( TagMessage tm );

    // Asynchronous result from saveFullImage().
    void
    saveImageResultCB( bool result );

private:

    std::unique_ptr < MessageListener > m_messageListener = nullptr;
};
}
}
}
