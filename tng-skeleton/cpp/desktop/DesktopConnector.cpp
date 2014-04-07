/**
 *
 **/

#include "DesktopConnector.h"
#include "common/misc.h"
#include <iostream>

DesktopConnector::DesktopConnector()
{
    // queued connection to prevent callbacks from firing inside setState
    connect( this, & DesktopConnector::stateChangedSignal,
             this, & DesktopConnector::stateChangedSlot,
             Qt::QueuedConnection );

    m_callbackNextId = 0;
}

bool DesktopConnector::initialize()
{
    return true;
}

void DesktopConnector::setState(const QString & path, const QString & newValue)
{
    QString oldVal = m_state[ path];
    if( oldVal == newValue) {
        // nothing to do
        return;
    }
    m_state[ path ] = newValue;
    std::cerr << "CPP setState " << path << "=" << newValue << "\n";
    emit stateChangedSignal( path, newValue);
}

QString DesktopConnector::getState(const QString & path)
{
    return m_state[ path ];
}

IConnector::CallbackID DesktopConnector::addCommandCallback(
        const QString & cmd,
        const IConnector::CommandCallback & cb)
{
    m_commandCallbackMap[cmd].push_back( cb);
    return m_callbackNextId++;
}

IConnector::CallbackID DesktopConnector::addStateCallback(
        IConnector::CSR path,
        const IConnector::StateChangedCallback & cb)
{
    m_stateCallbackList[ path].push_back( cb);
    return 1;
}

void DesktopConnector::registerView(IView *view)
{
    view->registration( this);
}

void DesktopConnector::refreshView(IView */*view*/)
{}

void DesktopConnector::jsSetStateSlot(const QString &key, const QString &value) {
    std::cerr << "jsSetState: " << key << "=" << value << "\n";
    setState( key, value);
}

void DesktopConnector::jsSendCommandSlot(const QString &cmd, const QString & parameter)
{
    auto & allCallbacks = m_commandCallbackMap[ cmd];
    QStringList results;
    for( auto & cb : allCallbacks) {
        results += cb( cmd, parameter, "1");
    }
    // tell javascript about result of the command
    emit jsCommandResultsSignal( results.join("|"));

}

void DesktopConnector::stateChangedSlot(const QString &key, const QString &value)
{
    // we just call our own callbacks here
    for( auto & cb : m_stateCallbackList[ key]) {
        cb( key, value);
    }
}
