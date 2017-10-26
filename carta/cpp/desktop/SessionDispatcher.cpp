/**
 *
 **/

#include "SessionDispatcher.h"
#include "CartaLib/LinearMap.h"
#include "core/MyQApp.h"
#include "core/SimpleRemoteVGView.h"
#include <iostream>
#include <QImage>
#include <QPainter>
#include <QXmlInputSource>
#include <cmath>
#include <QTime>
#include <QTimer>
#include <QCoreApplication>
#include <functional>

#include <QStringList>

#include <thread>
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include "websocketclientwrapper.h"
#include "websockettransport.h"
#include "qwebchannel.h"
#include <QBuffer>
#include <QThread>

#include "NewServerConnector.h"

void SessionDispatcher::startWebSocketChannel(){

    int port = 4317;

    // setup the QWebSocketServer
    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("QWebChannel Standalone Example Server"), QWebSocketServer::NonSecureMode, this);
    if (!m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        qFatal("Failed to open web socket server.");
        return;
    }

    qDebug() << "SessionDispatcher listening on port" << port;

    // wrap WebSocket clients in QWebChannelAbstractTransport objects
    m_clientWrapper = new WebSocketClientWrapper(m_pWebSocketServer);

    // setup the channel
    m_channel = new QWebChannel();
    QObject::connect(m_clientWrapper, &WebSocketClientWrapper::clientConnected,
                     m_channel, &QWebChannel::connectTo);

    m_channel->registerObject(QStringLiteral("QConnector"), this);
}

SessionDispatcher::SessionDispatcher()
{
    //Qt's built-in WebSocket + QWebChannel
    m_pWebSocketServer = nullptr;
    m_clientWrapper = nullptr;
    m_channel = nullptr;
    startWebSocketChannel();

}

SessionDispatcher::~SessionDispatcher()
{
    m_pWebSocketServer->close();

    if (m_pWebSocketServer != nullptr) {
        delete m_pWebSocketServer;
    }

    if (m_clientWrapper != nullptr) {
        delete m_clientWrapper;
    }

    if (m_channel != nullptr) {
        delete m_channel;
    }
}

void SessionDispatcher::jsSendCommandSlot(const QString & sessionID, const QString &cmd, const QString & parameter)
{
    // forward commands
    NewServerConnector *connector = static_cast<NewServerConnector*>(getConnectorInMap(sessionID));
    if (connector != nullptr){
        emit connector->jsSendCommandSignal(sessionID, cmd, parameter);
    }
    return;
}

IConnector* SessionDispatcher::getConnectorInMap(const QString & sessionID){

    mutex.lock();
    auto iter = clientList.find(sessionID);

    if(iter != clientList.end()) {
        qDebug()<<"Find NewServerConnector.h is";
        auto connector = iter->second;
        mutex.unlock();
        return connector;
    }

    mutex.unlock();
    qDebug()<<"Do not Find NewServerConnector.h"<<endl;
    return nullptr;
}

void SessionDispatcher::setConnectorInMap(const QString & sessionID, IConnector *connector){
    mutex.lock();
    clientList[sessionID] = connector;
    mutex.unlock();
}

void SessionDispatcher::jsViewUpdatedSignalForwardSlot(const QString & sessionID, const QString & viewName, const QString & img, qint64 id){
    emit jsViewUpdatedSignal(sessionID, viewName, img, id);
}


void SessionDispatcher::jsCommandResultsSignalForwardSlot(const QString & sessionID, const QString & cmd, const QString & results, const QString & subIdentifier){
    emit jsCommandResultsSignal(sessionID, cmd, results, subIdentifier);
}

//TODO implement later
void SessionDispatcher::jsSendKeepAlive(){
//    qDebug() << "get keepalive packet !!!!";
}

void SessionDispatcher::newSessionCreatedSlot(const QString & sessionID)
{
    // at this point it's safe to start using setState as the javascript
    // connector has registered to listen for the signal
    qDebug() << "new Client Session !!!!";

    if (getConnectorInMap(sessionID) != nullptr){
        qDebug()<<"Find, the value is !!!!!";
        return;
    }

//    auto iter = clientList.find(sessionID);

//    if(iter != clientList.end()) {
//        qDebug()<<"Find, the value is";
//        auto connector = iter->second;
//    } else {
        qDebug()<<"Do not Find"<<endl;

//        clientList.insert( std::make_pair(SessionID, new DesktopConnector));

        NewServerConnector *connector =  new NewServerConnector();
//        IConnector *connector2 = connector;
//        clientList[sessionID] = connector2;
        setConnectorInMap(sessionID, connector);

        connect(connector, SIGNAL(startViewerSignal(const QString &)), connector, SLOT(startViewerSlot(const QString &)));
        connect(connector,
                SIGNAL(jsSendCommandSignal(const QString &, const QString &, const QString &)),
                connector,
                SLOT(jsSendCommandSlot(const QString &, const QString &, const QString &)));

        // setup view size
        connect(connector,
                SIGNAL(jsUpdateViewSizeSignal(const QString &, const QString &, int, int) ),
                connector,
                SLOT( jsUpdateViewSizeSlot(const QString &, const QString &, int, int)));


        connect(connector,
                SIGNAL(jsCommandResultsSignal(const QString &, const QString &, const QString &, const QString &)),
                this,
                SLOT(jsCommandResultsSignalForwardSlot(const QString &, const QString &, const QString &, const QString &))
                );

        connect(connector,
                SIGNAL(jsViewUpdatedSignal(const QString &,  const QString &, const QString &, qint64)),
                this,
                SLOT(jsViewUpdatedSignalForwardSlot(const QString &, const QString &, const QString &, qint64))
                );

        // create a simple thread
        QThread* newThread = new QThread();
//        connector->selfThread = newThread;

        // let the new thread handle its events
        connector->moveToThread(newThread);
        newThread->setObjectName(sessionID);

        // start new thread's event loop
        newThread->start();

        //trigger signal
//        emit startViewerSignal(sessionID);
        emit connector->startViewerSignal(sessionID);
        int k2=0;

//    }

//    int k=0;

    // TODO: Thread 內部可以有收keep alive, 沒有就自己stop機制.
    // 所以何時需要外部stop, 應該是有ui上面的session list, delete時
    // 所以map的方法需要access到thread的pointer, 存在 DesktopConnector裡面?

    // x TODO: signal /slot到時應該是block的, 要check, 及想辦法避免lock, defer?

    // time to call the initialize callback
//    m_initializeCallback(SessionID);
//    m_initializeCallback(true);

    // 要改5, lock on this map. Also, viewer.start shoudl be on that thread.
    // x 要改3. viewer 跟此deksconnector都要move到那個thread, 這樣signal/slot跟event loop才是那個thread
    // x 要改4. 如何讓viewer活著????? 放在 Desktopconnector底下好了
    // 要改1. 是這裡new出後, 要設定sessionDispatcher跟他們的signal/slot, 1對多怎麼指定? trigger時多帶session id,
    // desktopconnector各自的slot會去filter. 是用a. thread<-這個好了 or b. 存起來呢?.
    //

    ////        viewer.start(); 裡面產生viewmanager, objec new出時會去access global的desktopconnector, 註冊callback function
    /// // 是用 Globals::instance()->connector(); <-要改2.
    /// //要不要放在map裡這件事, 應該是要, 不然
    /// //a. 會leak b. 如果有一樣的session上來. 或是
    /// //b. 收command時要檢查有沒有加過, 也有可能是cpp server restart, 此時會有command上來但沒有紀錄, 要new

    ////        1 session 對到1個 desktopconnector, 在不同thread裡.
    ////        都發signal到 desktopconnector
    ////        cpp算完 -> desktopconnector -> 再發signal 到sessionDispatcher -> 再送給js
    ////        globals ? 要改一點地方
    ////        desktopplatform? check一下

    ////        viewer? 每個人有自己一份好了
    ////        objectmanager, 維持原樣
    //         //viewermanger, 每個人有自己一份
    //        // plugin -> prepare 已提早
}

void SessionDispatcher::jsUpdateViewSizeSlot(const QString & sessionID, const QString & viewName, int width, int height)
{
    // forward commands
    NewServerConnector *connector = static_cast<NewServerConnector*>(getConnectorInMap(sessionID));
    if (connector != nullptr){
        emit connector->jsUpdateViewSizeSignal(sessionID, viewName, width, height);
    }

    return;
}

//********* will comment the below later

void SessionDispatcher::initialize(const InitializeCallback & cb)
{
}

void SessionDispatcher::setState(const QString& path, const QString & newValue)
{

}

QString SessionDispatcher::getState(const QString & path  )
{
    return "";
}

QString SessionDispatcher::getStateLocation( const QString& saveName ) const {
    return "";
}

IConnector::CallbackID SessionDispatcher::addCommandCallback(
        const QString & cmd,
        const IConnector::CommandCallback & cb)
{
    return 0;
}

IConnector::CallbackID SessionDispatcher::addStateCallback(
        IConnector::CSR path,
        const IConnector::StateChangedCallback & cb)
{
    return 0;
}

void SessionDispatcher::registerView(IView * view)
{

}

void SessionDispatcher::unregisterView( const QString& viewName ){
}

qint64 SessionDispatcher::refreshView(IView * view)
{
    return 0;
}

void SessionDispatcher::removeStateCallback(const IConnector::CallbackID & /*id*/)
{
    qFatal( "not implemented");
}

Carta::Lib::IRemoteVGView * SessionDispatcher::makeRemoteVGView(QString viewName)
{
    return nullptr;
}
//*********
