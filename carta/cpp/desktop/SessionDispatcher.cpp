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


#include "DesktopConnector.h"

/// \brief internal class of SessionDispatcher, containing extra information we like
///  to remember with each view
///
struct SessionDispatcher::ViewInfo
{

    /// pointer to user supplied IView
    /// this is a NON-OWNING pointer
    IView * view;

    /// last received client size
    QSize clientSize;

    /// linear maps convert x,y from client to image coordinates
    Carta::Lib::LinearMap1D tx, ty;

    /// refresh timer for this object
    QTimer refreshTimer;

    /// refresh ID
    qint64 refreshId = -1;

    ViewInfo( IView * pview )
    {
        view = pview;
        clientSize = QSize(1,1);
        refreshTimer.setSingleShot( true);
        // just long enough that two successive calls will result in only one redraw :)
        refreshTimer.setInterval( 1000 / 120);
    }

};

// not use now
// uWebSockets part, comment now, change to use qt's built-in WebSocket
//TODO Grimmer: this is for new CARTA, and is using hacked way to workaround passing command/object id/callback issue.
// void SessionDispatcher::startWebSocketServer() {

//    std::cout << "websocket starts running" << std::endl;
//    uWS::Hub h;
//    // DesktopConnector *conn = this;
//    h.onMessage([this](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
//        std::cout << "get something, echo back:" << message << std::endl;
//        //        ws->send(message, length, opCode);

//        char *psz = new char[length];
//        strncpy(psz, message, length);
//        QString message2(psz);
//        delete [] psz;

//        qDebug() << "convert to:"<< message2;

//        if (message2.contains("REQUEST_FILE_LIST")) {

//            // if ( )
//            QString command = "/CartaObjects/DataLoader:getData";
//            QString parameter = "path:";

////            QMetaObject::invokeMethod( this, "jsSendCommandSlot2", Qt::QueuedConnection );
//            //            conn->jsSendCommandSlot(command, parameter);

//            // QMetaObject::invokeMethod(this, "quit",
//            //               Qt::QueuedConnection);

////             pseudoJsSendCommandSlot(ws, opCode, command, parameter);

//            // SessionDispatcher::jsSendCommandSlot(const QString &cmd, const QString & parameter)
//            //    /CartaObjects/DataLoader:getData
//            //    path:
//        } else if (message2.contains("SELECT_FILE_TO_OPEN")) {

//            QStringList myStringList = message2.split(';');
//            if(myStringList.size()>=2){
//              auto fileName = myStringList[1];
//              qDebug()<< "fileName:" << myStringList[1];

//              QString command = "/CartaObjects/ViewManager:dataLoaded";
//              QString parameter = "id:/CartaObjects/c14,data:" + fileName;

////              pseudoJsSendCommandSlot(ws, opCode, command, parameter);

////              qDebug()<<"cmd:"<<cmd;
////              qDebug()<<"parameter:"<< parameter;
////              if (cmd=="/CartaObjects/ViewManager:dataLoaded") {
////                  if (parameter=="id:/CartaObjects/c14,data:/Users/grimmer/CARTA/Images/aJ.fits") {
////                      int kkk =0;
////                  }
////              }
//            }

//                            //            ws->send("hello", 5, opCode);
//        } else {
////            ws->send("hello", 5, opCode);
//        }
//    });
//    h.listen(4314);
//    h.run(); // will block here

//    std::cout << "websocket ends running" << std::endl;
// }

//void SessionDispatcher::receiveNewSession(int SessionID){


//}

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
    // queued connection to prevent callbacks from firing inside setState
    // connect( this, & SessionDispatcher::stateChangedSignal,
    //          this, & SessionDispatcher::stateChangedSlot,
    //          Qt::QueuedConnection );
    //
    // m_callbackNextId = 0;

    // test1: uWebSocket part
//    std::thread mThread( &SessionDispatcher::startWebSocketServer, this );
//    mThread.detach();

    // test2: change to use Qt's buint-in WebSocket
    // https://github.com/GarageGames/Qt/blob/master/qt-5/qtwebsockets/examples/websockets/echoserver/echoserver.cpp
//     m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Echo Server"),
//                                             QWebSocketServer::NonSecureMode, this);
//     m_debug = true;
//     int port = 4317;
//     if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
//         if (m_debug)
//             qDebug() << "DesktopConnector listening on port" << port;
//         connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
//                 this, &SessionDispatcher::onNewConnection);
// //        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &SessionDispatcher::closed);
//     }

//test3, Qt's built-in WebSocket + QWebChannel
    m_pWebSocketServer = nullptr;
    m_clientWrapper = nullptr;
    m_channel = nullptr;
    startWebSocketChannel();

}

SessionDispatcher::~SessionDispatcher()
{
    m_pWebSocketServer->close();
//    qDeleteAll(m_clients.begin(), m_clients.end());

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

void SessionDispatcher::initialize(const InitializeCallback & cb)
{
    m_initializeCallback = cb;
}

void SessionDispatcher::jsSetStateSlot(const QString & key, const QString & value) {
    // it's ok to call setState directly, because callbacks will be invoked
    // from there asynchronously
    setState( key, value );

    if( CARTA_RUNTIME_CHECKS) {
        auto iter = m_stateCallbackList.find( key);
        if( iter == m_stateCallbackList.end()) {
            qWarning() << "JS setState has no listener" << key << "=" << value;
        }
    }
}

void SessionDispatcher::jsSendCommandSlot(const QString & sessionID, const QString &cmd, const QString & parameter)
{
    // forward commands
    emit jsSendCommandSignal(sessionID, cmd, parameter);
    return;

//    /CartaObjects/DataLoader:getData
//    path:

//    qDebug()<<"cmd:"<<cmd;
//    qDebug()<<"parameter:"<< parameter;
//    if (cmd=="/CartaObjects/c14:setZoomLevel") {
//        int kkk =0;
//    }

    // call all registered callbacks and collect results, but asynchronously
//    defer( [cmd, parameter, this ]() {
//        auto & allCallbacks = m_commandCallbackMap[ cmd];
//        QStringList results;
//        for( auto & cb : allCallbacks) {
//            results += cb( cmd, parameter, "1"); // session id fixed to "1"
//        }

//        // pass results back to javascript
//        emit jsCommandResultsSignal( cmd, results.join("|"));

//        if( allCallbacks.size() == 0) {
//            qWarning() << "JS command has no server listener:" << cmd << parameter;
//        }
//    });
}

//TODO add lock/mutex later
IConnector* SessionDispatcher::getConnectorInMap(const QString & sessionID){

    mutex.lock();
    auto iter = clientList.find(sessionID);

    if(iter != clientList.end()) {
        qDebug()<<"Find Desktopconnector is";
        auto connector = iter->second;
        mutex.unlock();
        return connector;
    }

    mutex.unlock();
    qDebug()<<"Do not Find Desktopconnector"<<endl;
    return nullptr;
}

//TODO add lock/mutex later
void SessionDispatcher::setConnectorInMap(const QString & sessionID, IConnector *connector){
    mutex.lock();
    clientList[sessionID] = connector;
    mutex.unlock();

}

void SessionDispatcher::jsCommandResultsSignalForwardSlot(const QString & sessionID, const QString & cmd, const QString & results){
    emit jsCommandResultsSignal(sessionID, cmd, results);
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

        DesktopConnector *connector =  new DesktopConnector();
//        IConnector *connector2 = connector;
//        clientList[sessionID] = connector2;
        setConnectorInMap(sessionID, connector);

        connect(this, SIGNAL(startViewerSignal(const QString &)), connector, SLOT(startViewerSlot(const QString &)));
        connect(this,
                SIGNAL(jsSendCommandSignal(const QString &, const QString &, const QString &)),
                connector,
                SLOT(jsSendCommandSlot(const QString &, const QString &, const QString &)));
        connect(connector,
                SIGNAL(jsCommandResultsSignal(const QString &, const QString &, const QString &)),
                this,
                SLOT(jsCommandResultsSignalForwardSlot(const QString &, const QString &, const QString &))
                );

        // create a simple thread
        QThread* newThread = new QThread();
        connector->selfThread = newThread;

        // let the new thread handle its events
        connector->moveToThread(newThread);
        newThread->setObjectName(sessionID);

        // start new thread's event loop
        newThread->start();

        //trigger signal
        emit startViewerSignal(sessionID);
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

SessionDispatcher::ViewInfo * SessionDispatcher::findViewInfo( const QString & viewName)
{
    auto viewIter = m_views.find( viewName);
    if( viewIter == m_views.end()) {
        qWarning() << "SessionDispatcher::findViewInfo: Unknown view " << viewName;
        return nullptr;
    }

    return viewIter-> second;
}



void SessionDispatcher::jsUpdateViewSlot(const QString & viewName, int width, int height)
{
    ViewInfo * viewInfo = findViewInfo( viewName);
    if( ! viewInfo) {
        qWarning() << "Received update for unknown view " << viewName;
        return;
    }

    IView * view = viewInfo-> view;
    viewInfo-> clientSize = QSize( width, height);

    defer([this,view,viewInfo](){
        view-> handleResizeRequest( viewInfo-> clientSize);
        refreshView( view);
    });
}

void SessionDispatcher::jsViewRefreshedSlot(const QString & viewName, qint64 id)
{
    //qDebug() << "jsViewRefreshedSlot()" << viewName << id;
    ViewInfo * viewInfo = findViewInfo( viewName);
    if( ! viewInfo) {
        qCritical() << "Received refresh view signal for unknown view" << viewName;
        return;
    }
    CARTA_ASSERT( viewInfo-> view);
    viewInfo-> view-> viewRefreshed( id);
}

void SessionDispatcher::jsMouseMoveSlot(const QString &viewName, int x, int y)
{
    ViewInfo * viewInfo = findViewInfo( viewName);
    if( ! viewInfo) {
        qWarning() << "Received mouse event for unknown view " << viewName << "\n";
        return;
    }

    IView * view = viewInfo-> view;

    // we need to map x,y from screen coordinates to image coordinates
    int xi = std::round( viewInfo-> tx(x));
    int yi = std::round( viewInfo-> ty(y));

    // tell the view about the event
    QMouseEvent ev( QEvent::MouseMove,
                    QPoint(xi,yi),
                    Qt::NoButton,
                    Qt::NoButton,
                    Qt::NoModifier   );
    view-> handleMouseEvent( ev);
}

void SessionDispatcher::stateChangedSlot(const QString & key, const QString & value)
{
    // find the list of callbacks for this path
    auto iter = m_stateCallbackList.find( key);

    // if it does not exist, do nothing
    if( iter == m_stateCallbackList.end()) {
        return;
    }

    // call all registered callbacks for this key
    iter-> second-> callEveryone( key, value);
}

//********* will comment the below later

void SessionDispatcher::setState(const QString& path, const QString & newValue)
{
    // find the path
    auto it = m_state.find( path);

    // if we cannot find it, insert it, together with the new value, and emit a change
    if( it == m_state.end()) {
        m_state[path] = newValue;
        emit stateChangedSignal( path, newValue);
        return;
    }

    // if we did find it, but the value is different, set it to new value and emit signal
    if( it-> second != newValue) {
        it-> second = newValue;
        emit stateChangedSignal( path, newValue);
    }

    // otherwise there was no change to state, so do dothing
}


QString SessionDispatcher::getState(const QString & path  )
{
    return m_state[ path ];
}


// Return the location where the state is saved.
QString SessionDispatcher::getStateLocation( const QString& saveName ) const {
	// \todo Generalize this.
	return "/tmp/"+saveName+".json";
}

IConnector::CallbackID SessionDispatcher::addCommandCallback(
        const QString & cmd,
        const IConnector::CommandCallback & cb)
{
    m_commandCallbackMap[cmd].push_back( cb);
    return m_callbackNextId++;
}

IConnector::CallbackID SessionDispatcher::addStateCallback(
        IConnector::CSR path,
        const IConnector::StateChangedCallback & cb)
{
    // find the list of callbacks for this path
    auto iter = m_stateCallbackList.find( path);

    // if it does not exist, create it
    if( iter == m_stateCallbackList.end()) {
//        qDebug() << "Creating callback list for variable " << path;
        auto res = m_stateCallbackList.insert( std::make_pair(path, new StateCBList));
        iter = res.first;
    }

//    iter = m_stateCallbackList.find( path);
//    if( iter == m_stateCallbackList.end()) {
////        qDebug() << "What the hell";
//    }

    // add the calllback
    return iter-> second-> add( cb);

//    return m_stateCallbackList[ path].add( cb);
}

void SessionDispatcher::registerView(IView * view)
{
    // let the view know it's registered, and give it access to the connector
    view->registration( this);

    // insert this view int our list of views
    ViewInfo * viewInfo = new ViewInfo( view);
//    viewInfo-> view = view;
//    viewInfo-> clientSize = QSize(1,1);
    m_views[ view-> name()] = viewInfo;

    // connect the view's refresh timer to a lambda, which will in turn call
    // refreshViewNow()
    // this is instead of using std::bind...
    connect( & viewInfo->refreshTimer, & QTimer::timeout,
            [=] () {
                     refreshViewNow( view);
    });
}

//unregister the view
void SessionDispatcher::unregisterView( const QString& viewName ){
    ViewInfo* viewInfo = this->findViewInfo( viewName );
    if ( viewInfo != nullptr ){

        (& viewInfo->refreshTimer)->disconnect();
        m_views.erase( viewName );
    }
}

   static QTime st;

//schedule a view refresh
qint64 SessionDispatcher::refreshView(IView * view)
{
    // find the corresponding view info
    ViewInfo * viewInfo = findViewInfo( view-> name());
    if( ! viewInfo) {
        // this is an internal error...
        qCritical() << "refreshView cannot find this view: " << view-> name();
        return -1;
    }

    // start the timer for this view if it's not already started
//    if( ! viewInfo-> refreshTimer.isActive()) {
//        viewInfo-> refreshTimer.start();
//    }
//    else {
//        qDebug() << "########### saved refresh for " << view->name();
//    }

    refreshViewNow(view);

    viewInfo-> refreshId ++;
    return viewInfo-> refreshId;
}

void SessionDispatcher::removeStateCallback(const IConnector::CallbackID & /*id*/)
{
    qFatal( "not implemented");
}


Carta::Lib::IRemoteVGView * SessionDispatcher::makeRemoteVGView(QString viewName)
{
//    return new Carta::Core::SimpleRemoteVGView( this, viewName, this);
    return nullptr;
}


void SessionDispatcher::refreshViewNow(IView *view)
{
    static int enterCount =0;

    enterCount++;

//    if (enterCount>1) {
//        return;
//    }


    ViewInfo * viewInfo = findViewInfo( view-> name());
    if( ! viewInfo) {
        // this is an internal error...
        qCritical() << "refreshView cannot find this view: " << view-> name();
        return;
    }
    // get the image from view
    const QImage & origImage = view-> getBuffer();

    QSize clientImageSize = viewInfo->clientSize;
    if( origImage.size() != clientImageSize && clientImageSize.height() > 0 &&
            clientImageSize.width() > 0 && origImage.height() > 0 ) {
        qDebug() << "Having to re-scale the image, this is slow" << origImage.size() << viewInfo->clientSize;
        // scale the image to fit the client size, in case it wasn't scaled alerady
        QImage destImage = origImage.scaled(
                               viewInfo->clientSize, Qt::KeepAspectRatio,
                               //                Qt::SmoothTransformation);
                               Qt::FastTransformation);
        // calculate the offset needed to center the image
        int xOffset = (viewInfo-> clientSize.width() - destImage.size().width())/2;
        int yOffset = (viewInfo-> clientSize.height() - destImage.size().height())/2;
        QImage pix( viewInfo->clientSize, QImage::Format_ARGB32_Premultiplied);
        pix.fill( qRgba( 0, 0, 0, 0));
        QPainter p( & pix);
        p.setCompositionMode( QPainter::CompositionMode_Source);
        p.drawImage( xOffset, yOffset, destImage );

        // remember the transformations we did to the image in the viewInfo so that we can
        // properly translate mouse events etc
        viewInfo-> tx = Carta::Lib::LinearMap1D( xOffset, xOffset + destImage.size().width()-1,
                                     0, origImage.width()-1);
        viewInfo-> ty = Carta::Lib::LinearMap1D( yOffset, yOffset + destImage.size().height()-1,
                                     0, origImage.height()-1);

        QString nname = view-> name();

        qDebug()<<"grimmer image11111";

//        emit jsViewUpdatedSignal( view-> name(), pix, viewInfo-> refreshId);
//        return;

        QImage *finalImage;

        finalImage = &pix;

        if (finalImage) {
//            image.load("test.png");
           QByteArray byteArray;
           QBuffer buffer(&byteArray);
           finalImage->save(&buffer, "JPEG", 90); // writes the image in PNG format inside the buffer
           QString base64Str = QString::fromLatin1(byteArray.toBase64().data());
           emit jsViewUpdatedSignal( view-> name(), base64Str, viewInfo-> refreshId);

//           QString jsonStr = "{\"cmd\":\"SELECT_FILE_TO_OPEN\",\"image\":\""+base64Str+"\"}";
//           if (test_pClient != nullptr) {
//               qint64 sendNumber = test_pClient->sendTextMessage(jsonStr);
//               test_pClient->flush();
//               qDebug() << "send:"<<sendNumber;
//           }
        } else {
            qDebug() << "grimmer not ready";
        }

    }
    else {

        static int count = 0;
        count++;

        qDebug()<<"grimmer image2222:"<< count;

        QString nname = view-> name();

        viewInfo-> tx = Carta::Lib::LinearMap1D( 0, 1, 0, 1);
        viewInfo-> ty = Carta::Lib::LinearMap1D( 0, 1, 0, 1);

//        emit jsViewUpdatedSignal( view-> name(), origImage, viewInfo-> refreshId);
//        return;

        const QImage *finalImage = &origImage;
        if (finalImage) {
//            image.load("test.png");
           QByteArray byteArray;
           QBuffer buffer(&byteArray);
           finalImage->save(&buffer, "JPEG", 90); // writes the image in PNG format inside the buffer. 50 is a little bad
           QString base64Str = QString::fromLatin1(byteArray.toBase64().data());
           emit jsViewUpdatedSignal( view-> name(), base64Str,  viewInfo-> refreshId);

//           QString jsonStr = "{\"cmd\":\"SELECT_FILE_TO_OPEN\",\"image\":\""+base64Str+"\"}";
//           if (test_pClient != nullptr) {
//               qint64 sendNumber = test_pClient->sendTextMessage(jsonStr);
//               test_pClient->flush();
//               qDebug() << "send:"<<sendNumber;
//           }
        } else {
            qDebug() << "grimmer not ready";
        }

    }



    //http://techqa.info/programming/question/29295074/Converting-a-QImage-to-a-C--Image

    // test webSocket case
    auto sendBackToWSClient = [this](const QImage & img) {

    // 1. convert Qimage to jpeg base64 string


    // 2.d send
    //test_pClient->sendTextMessage();

    };




}
