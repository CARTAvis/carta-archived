#ifndef SERVERCONNECTOR_H
#define SERVERCONNECTOR_H

#include "common/IConnector.h"

#include "CSI/Standard/CsiStandard.h"
#include "CSI/Standard/CsiThreading.h"
#include "CSI/PureWeb/StateManager/StateManager.h"
#include <QObject>
#include <QEvent>
#include <QApplication>
#include <iostream>
#include <unordered_set>


class QtMessageTickler
        : public QObject
        , public CSI::PureWeb::Server::IResponseProvider
        , public CSI::PureWeb::Server::IStateManagerPlugin
{
    Q_OBJECT
public:
    QtMessageTickler() {
        m_eventCounter = 0;
    }
    bool event(QEvent * event) {
        if (event->type() == QEvent::User) {
            CSI::Threading::Interlocked::Exchange(&m_eventCounter, 0);
            CSI::Threading::UiDispatcher::PumpMessages();
            return true;
        }
        return false;
    }
    void SessionConnected(CSI::Guid sessionId, CSI::Typeless const& /*command*/) {
        m_pStateManager->SessionManager().AddResponseProvider(sessionId, this);
    }
    void SessionDisconnected(CSI::Guid sessionId, CSI::Typeless const& /*command*/) {
        m_pStateManager->SessionManager().RemoveResponseProvider(sessionId, this);
    }
    CSI::Collections::List<CSI::PureWeb::Server::ResponseInfo> GetNextResponses(CSI::Guid) {
        if (CSI::Threading::Interlocked::CompareExchange(&m_eventCounter, 1, 0)) {
            QEvent * event = new QEvent(QEvent::User);
            QApplication::postEvent(this,event);
        }
        return m_empty;
    }
    void Initialize(CSI::PureWeb::Server::StateManager * pStateManager) {
        if (pStateManager == NULL)
            CSI_THROW(CSI::ArgumentNullException, "pStateManager");
        m_pStateManager = pStateManager;
    }

    void Uninitialize() { ; }

private:
    CSI::Collections::List<CSI::PureWeb::Server::ResponseInfo> m_empty;
    CSI::PureWeb::Server::StateManager * m_pStateManager;
    int m_eventCounter;
};


class ServerConnector : public QObject, public IConnector
{
    Q_OBJECT
public:

    explicit ServerConnector( int argc, char ** argv);

    virtual bool initialize() Q_DECL_OVERRIDE;
    virtual void setState(const QString & path, const QString & value) Q_DECL_OVERRIDE;
    virtual QString getState(const QString &path) Q_DECL_OVERRIDE;
    virtual CallbackID addCommandCallback( const QString & cmd, const CommandCallback & cb) Q_DECL_OVERRIDE;

    static void OnPureWebShutdown(CSI::PureWeb::Server::StateManagerServer&,
                                  CSI::EmptyEventArgs&);

    void genericCommandListener(CSI::Guid sessionid, CSI::Typeless const& command, CSI::Typeless& responses);


    CSI::CountedPtr<CSI::PureWeb::Server::StateManagerServer> m_server;
    CSI::CountedPtr<CSI::PureWeb::Server::StateManager> m_stateManager;
    CallbackID m_callbackNextId; //  = 0;

    typedef std::vector<CommandCallback> CommandCallbackList;
    std::map<QString,  CommandCallbackList> m_commandCallbackMap;
    typedef std::vector<StateChangedCallback> StateCBList;
    std::map<QString, StateCBList> m_stateCallbackList;
    bool m_startServer;

    // set to tell us whether a pureweb statechange callback is registered or not
    std::unordered_set< std::string > m_pwStateCBset;


    virtual CallbackID addStateCallback(CSR path, const StateChangedCallback &cb) Q_DECL_OVERRIDE;

    // internal callback for pureweb value changed event
    void pureWebValueChangedCB( const CSI::ValueChangedEventArgs & val);

    virtual void registerView(IView * view) Q_DECL_OVERRIDE;
    virtual void refreshView(IView *view) Q_DECL_OVERRIDE;
    virtual void removeStateCallback( const CallbackID & id);

//signals:
//    void delayedInternalValueChangedSignal( CSI::ValueChangedEventArgs);

//protected slots:
//    void delayedInternalValueChangedCB( CSI::ValueChangedEventArgs);

};

#endif // SERVERCONNECTOR_H
