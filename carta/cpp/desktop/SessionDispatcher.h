/**
 *
 **/


#ifndef SESSION_DISPATCHER_H
#define SESSION_DISPATCHER_H

#include <QObject>
#include <qmutex.h>
#include "NewServerConnector.h"

#include "core/IConnector.h"
#include "CartaLib/IRemoteVGView.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)

QT_FORWARD_DECLARE_CLASS(WebSocketClientWrapper)
QT_FORWARD_DECLARE_CLASS(QWebChannel)

class IView;

class SessionDispatcher : public QObject, public IConnector
{
    Q_OBJECT
public:

    /// constructor
    explicit SessionDispatcher();

    //** will comment later
    // implementation of IConnector interface
    virtual void initialize( const InitializeCallback & cb) override;

    virtual CallbackID addCommandCallback( const QString & cmd, const CommandCallback & cb) override;
    virtual CallbackID addMessageCallback( const QString & cmd, const MessageCallback & cb) override;
    virtual CallbackID addStateCallback(CSR path, const StateChangedCallback &cb) override;

    virtual void setState(const QString& state, const QString & newValue) override;
    virtual QString getState(const QString&) override;
    virtual void registerView(IView * view) override;
    void unregisterView( const QString& viewName ) override;
    virtual qint64 refreshView( IView * view) override;
    virtual void removeStateCallback( const CallbackID & id) override;
    virtual Carta::Lib::IRemoteVGView * makeRemoteVGView( QString viewName) override;
    virtual QString getStateLocation( const QString& saveName ) const override;
    //**

    void startWebSocketChannel();
    QWebSocketServer *m_pWebSocketServer;
    WebSocketClientWrapper *m_clientWrapper;
    QWebChannel *m_channel;
     ~SessionDispatcher();
public slots:

    /// javascript calls this to set a state
//    void jsSetStateSlot( const QString & key, const QString & value);

    /// javascript calls this to send a command
    void jsSendCommandSlot(const QString & sessionID, const QString & senderSession, const QString & cmd, const QString & parameter);

    /// javascript calls this to let us know js connector is ready
    void newSessionCreatedSlot(const QString & sessionID);

    /// javascript calls this when view is resized
    void jsUpdateViewSizeSlot(const QString & sessionID, const QString & viewName, int width, int height);

    /// javascript calls this when the view is refreshed
//    void jsViewRefreshedSlot( const QString & viewName, qint64 id);

    /// javascript calls this on mouse move inside a view
    /// \deprecated
//    void jsMouseMoveSlot( const QString & viewName, int x, int y);

    /// this is the callback for stateChangedSignal
//    void stateChangedSlot( const QString & key, const QString & value);

    void jsCommandResultsSignalForwardSlot(const QString & sessionID, const QString & senderSession, const QString & cmd, const QString & results, const QString & subIdentifier);
    void jsViewUpdatedSignalForwardSlot(const QString & sessionID, const QString & viewName, const QString & img, qint64 id);
    void jsSendKeepAlive();

    // no use. Qt's built-in WebSocket, not we use its wrapper, QWebChannel made by Qt
//    void onNewConnection();
//    void processTextMessage(QString message);
//    void processBinaryMessage(QByteArray message);
//    void socketDisconnected();

signals:

    // void closed();

    /// we emit this signal when state is changed (either by c++ or by javascript)
    /// we listen to this signal, and so does javascript
    /// our listener then calls callbacks registered for this value
    /// javascript listener caches the new value and also calls registered callbacks
//    void stateChangedSignal( const QString & key, const QString & value);

    /// we emit this signal when command results are ready
    /// javascript listens to it
    void jsCommandResultsSignal(const QString & sessionID, const QString & senderSession, const QString & cmd, const QString & results, const QString & subIdentifier);

    /// emitted by c++ when we want javascript to repaint the view
    void jsViewUpdatedSignal(const QString & sessionID, const QString & viewName, const QString & img, qint64 id);

public:

    IConnector* getConnectorInMap(const QString & sessionID);
    void setConnectorInMap(const QString & sessionID, IConnector *connector);

protected:

    std::map<QString,  IConnector*> clientList;

private:
    QMutex mutex;
};


#endif // SESSION_DISPATCHER_H
