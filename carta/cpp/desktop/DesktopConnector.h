/**
 *
 **/


#ifndef DESKTOP_DESKTOPCONNECTOR_H
#define DESKTOP_DESKTOPCONNECTOR_H

#include <QObject>
#include "core/IConnector.h"
#include "core/CallbackList.h"
#include "CartaLib/IRemoteVGView.h"

//#include <uWS/uWS.h>

#include <QList>
#include <QByteArray>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

QT_FORWARD_DECLARE_CLASS(WebSocketClientWrapper)
QT_FORWARD_DECLARE_CLASS(QWebChannel)

class MainWindow;
class IView;

/// private info we keep with each view
/// unfortunately it needs to live as it's own class because we need to give it slots...
//class ViewInfo;

class DesktopConnector : public QObject, public IConnector
{
    Q_OBJECT
public:

    /// constructor
    explicit DesktopConnector();

    // implementation of IConnector interface
    virtual void initialize( const InitializeCallback & cb) override;
    virtual void setState(const QString& state, const QString & newValue) override;
    virtual QString getState(const QString&) override;
    virtual CallbackID addCommandCallback( const QString & cmd, const CommandCallback & cb) override;
    virtual CallbackID addStateCallback(CSR path, const StateChangedCallback &cb) override;
    virtual void registerView(IView * view) override;
    void unregisterView( const QString& viewName ) override;
    virtual qint64 refreshView( IView * view) override;
    virtual void removeStateCallback( const CallbackID & id) override;
    virtual Carta::Lib::IRemoteVGView *
    makeRemoteVGView( QString viewName) override;

    /// Return the location where the state is saved.
    virtual QString getStateLocation( const QString& saveName ) const;

    void startWebSocketServer();

//    void pseudoJsSendCommandSlot(const QString &cmd, const QString & parameter);

//    void jsSendCommandSlot2();
//    void pseudoJsSendCommandSlot(uWS::WebSocket<uWS::SERVER> *ws, uWS::OpCode opCode,  const QString &cmd, const QString & parameter);

    // Qt's built-in WebSocket
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    bool m_debug;
    ~DesktopConnector();
    void pseudoJsSendCommandSlot(const QString &cmd, const QString & parameter, QWebSocket *pClient);


    void startWebSocketChannel();
    // QWebSocketServer *m_pWebSocketServer;
    WebSocketClientWrapper *m_clientWrapper;
    QWebChannel *m_channel;
    // ~DesktopConnector();
public slots:

    /// javascript calls this to set a state
    void jsSetStateSlot( const QString & key, const QString & value);
    /// javascript calls this to send a command
    void jsSendCommandSlot( const QString & cmd, const QString & parameter);
    /// javascript calls this to let us know js connector is ready
    void jsConnectorReadySlot();
    /// javascript calls this when view is resized
    void jsUpdateViewSlot( const QString & viewName, int width, int height);
    /// javascript calls this when the view is refreshed
    void jsViewRefreshedSlot( const QString & viewName, qint64 id);
    /// javascript calls this on mouse move inside a view
    /// \deprecated
    void jsMouseMoveSlot( const QString & viewName, int x, int y);

    /// this is the callback for stateChangedSignal
    void stateChangedSlot( const QString & key, const QString & value);

    // Qt's built-in WebSocket
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

signals:

    void closed();


    /// we emit this signal when state is changed (either by c++ or by javascript)
    /// we listen to this signal, and so does javascript
    /// our listener then calls callbacks registered for this value
    /// javascript listener caches the new value and also calls registered callbacks
    void stateChangedSignal( const QString & key, const QString & value);
    /// we emit this signal when command results are ready
    /// javascript listens to it
    void jsCommandResultsSignal( const QString & cmd, const QString & results);
    /// emitted by c++ when we want javascript to repaint the view
    void jsViewUpdatedSignal( const QString & viewName, const QString & img, qint64 id);

public:

    typedef std::vector<CommandCallback> CommandCallbackList;
    std::map<QString,  CommandCallbackList> m_commandCallbackMap;

    // list of callbacks
    typedef CallbackList<CSR, CSR> StateCBList;

    /// for each state we maintain a list of callbacks
    std::map<QString, StateCBList *> m_stateCallbackList;

    /// IDs for command callbacks
    CallbackID m_callbackNextId;

    /// private info we keep with each view
    struct ViewInfo;

    /// map of view names to view infos
    std::map< QString, ViewInfo *> m_views;

    ViewInfo * findViewInfo(const QString &viewName);

    virtual void refreshViewNow(IView *view);

    /// @todo move as may of these as possible to protected section

protected:

    InitializeCallback m_initializeCallback;
    std::map< QString, QString > m_state;

};


#endif // DESKTOP_DESKTOPCONNECTOR_H
