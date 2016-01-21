#pragma once

#include "CartaLib/IRemoteVGView.h"
#include "core/IConnector.h"
#include "CSI/Standard/CsiStandard.h"
#include "CSI/Standard/CsiThreading.h"
#include "CSI/PureWeb/StateManager/StateManager.h"
#include <QObject>
#include <QEvent>
#include <QApplication>
#include <iostream>
#include <unordered_set>
#include <map>

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

class PWIViewConverter;

class ServerConnector : public QObject, public IConnector
{

    Q_OBJECT

public:

    /// constructor
    /// initializes internal state
    explicit ServerConnector();

    /// initialize connector
    /// establish connection to the client, parse url arguments
    virtual void initialize(const InitializeCallback & cb) Q_DECL_OVERRIDE;

    /// set a state 'path' to 'value'
    virtual void setState(const QString & path, const QString & value) Q_DECL_OVERRIDE;

    /// retrieve a state value
    virtual QString getState(const QString &path) Q_DECL_OVERRIDE;

    /// Return the location where the state is saved.
    virtual QString getStateLocation( const QString& saveName ) const;

    /// add a command callback
    virtual CallbackID addCommandCallback( const QString & cmd, const CommandCallback & cb) Q_DECL_OVERRIDE;

    /// add a state callback
    virtual CallbackID addStateCallback(CSR path, const StateChangedCallback &cb) Q_DECL_OVERRIDE;

    /// register view implementation
    virtual void registerView(IView * view) Q_DECL_OVERRIDE;

    /// unregister a view implementation
    virtual void unregisterView( const QString& viewName ) Q_DECL_OVERRIDE;

    /// refresh view implementation
    virtual qint64 refreshView( IView * view) override;

    /// remove state callback implementation
    virtual void removeStateCallback( const CallbackID & id) Q_DECL_OVERRIDE;

    /// get the initial file list (used by server platform)
    virtual const QStringList & initialFileList();

    /// returns a map of url encoded parameters that were used to invoke the main program
    /// this only works after initialize() was called
//    const std::map< QString, QString> & urlParams();

    virtual Carta::Lib::IRemoteVGView *
    makeRemoteVGView( QString viewName) override;

    virtual ~ServerConnector();

private:

    /// this gets called when pureweb server is shutting down
    static void OnPureWebShutdown(CSI::PureWeb::Server::StateManagerServer&,
                                  CSI::EmptyEventArgs&);

    /// internal true pureweb command listener, our command callback piggyback to this one
    void genericCommandListener(CSI::Guid sessionid, CSI::Typeless const& command, CSI::Typeless& responses);

    /// internal listener for view refresh commands from the clients
    void viewRefreshedCommandCB(CSI::Guid sessionid, CSI::Typeless const& command, CSI::Typeless& responses);

    /// pointer to the server
    CSI::CountedPtr<CSI::PureWeb::Server::StateManagerServer> m_server;

    /// pointer to the state manager
    CSI::CountedPtr<CSI::PureWeb::Server::StateManager> m_stateManager;

    /// callback ID counter, used to provide unique command callback IDs
    CallbackID m_callbackNextId; //  = 0;

    typedef std::vector<CommandCallback> CommandCallbackList;
    std::map<QString,  CommandCallbackList> m_commandCallbackMap;
    typedef std::vector<StateChangedCallback> StateCBList;
    std::map<QString, StateCBList> m_stateCallbackList;

    // set to tell us whether a pureweb statechange callback is registered or not
    std::unordered_set< std::string > m_pwStateCBset;

    // internal callback for pureweb value changed event
    void pureWebValueChangedCB( const CSI::ValueChangedEventArgs & val);

    // list of url encoded parameters
    std::map< QString, QString> m_urlParams;

    // initial file list
    QStringList m_initialFileList;

    // whether initialize was called
    bool m_initialized;

    void print( CSI::Typeless treeRoot ) const;

    std::map< QString, PWIViewConverter *> m_pwviews;
};

