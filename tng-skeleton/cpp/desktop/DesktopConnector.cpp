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
}

bool DesktopConnector::initialize()
{
    return true;
}

void DesktopConnector::setState(const QString & path, const QString & value)
{
    m_state[ path ] = value;
    std::cerr << "CPP setState " << path << "=" << value << "\n";
    emit stateChangedSignal( path, value);
}

QString DesktopConnector::getState(const QString & path)
{
    return m_state[ path ];
}

IConnector::CallbackID DesktopConnector::addCommandCallback(
        const QString & /*cmd*/,
        const IConnector::CommandCallback & /*cb*/)
{
    return 1;
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
    emit stateChangedSignal( key, value);
}

void DesktopConnector::stateChangedSlot(const QString &key, const QString &value)
{
    // we just call our own callbacks here
    for( auto & cb : m_stateCallbackList[ key]) {
        cb( key, value);
    }
}
