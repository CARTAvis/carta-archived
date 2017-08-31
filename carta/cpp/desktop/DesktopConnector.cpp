/**
 *
 **/

#include "DesktopConnector.h"
#include "CartaLib/LinearMap.h"
#include "core/MyQApp.h"
#include "core/SimpleRemoteVGView.h"
#include "core/State/ObjectManager.h"
#include "core/Data/DataLoader.h"
#include "core/Data/ViewManager.h"
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


/// \brief internal class of DesktopConnector, containing extra information we like
///  to remember with each view
///
struct DesktopConnector::ViewInfo
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

void DesktopConnector::testStartViewerSlot(const QString & sessionID) {


    emit startViewerSignal(sessionID);

}


// not use now
// uWebSockets part, comment now, change to use qt's built-in WebSocket
//TODO Grimmer: this is for new CARTA, and is using hacked way to workaround passing command/object id/callback issue.
void DesktopConnector::startWebSocketServer() {

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

//            // DesktopConnector::jsSendCommandSlot(const QString &cmd, const QString & parameter)
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
}


void DesktopConnector::startWebSocketChannel(){

    int port = 4317;

    // setup the QWebSocketServer
    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("QWebChannel Standalone Example Server"), QWebSocketServer::NonSecureMode, this);
    if (!m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        qFatal("Failed to open web socket server.");
        return;
    }

    qDebug() << "DesktopConnector listening on port" << port;

    // wrap WebSocket clients in QWebChannelAbstractTransport objects
    m_clientWrapper = new WebSocketClientWrapper(m_pWebSocketServer);

    // setup the channel
    m_channel = new QWebChannel();
    QObject::connect(m_clientWrapper, &WebSocketClientWrapper::clientConnected,
                     m_channel, &QWebChannel::connectTo);

    m_channel->registerObject(QStringLiteral("QConnector"), this);
}

DesktopConnector::DesktopConnector()
{
    // queued connection to prevent callbacks from firing inside setState
    connect( this, & DesktopConnector::stateChangedSignal,
             this, & DesktopConnector::stateChangedSlot,
             Qt::QueuedConnection );

    m_callbackNextId = 0;

    // test1: uWebSocket part
//    std::thread mThread( &DesktopConnector::startWebSocketServer, this );
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
//                 this, &DesktopConnector::onNewConnection);
// //        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &DesktopConnector::closed);
//     }

//test3, Qt's built-in WebSocket + QWebChannel
    m_pWebSocketServer = nullptr;
    m_clientWrapper = nullptr;
    m_channel = nullptr;
    selfThread = nullptr;
//    startWebSocketChannel();

}

DesktopConnector::~DesktopConnector()
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

void DesktopConnector::initialize(const InitializeCallback & cb)
{
    m_initializeCallback = cb;
}

void DesktopConnector::setState(const QString& path, const QString & newValue)
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


QString DesktopConnector::getState(const QString & path  )
{
    return m_state[ path ];
}


/// Return the location where the state is saved.
QString DesktopConnector::getStateLocation( const QString& saveName ) const {
	// \todo Generalize this.
	return "/tmp/"+saveName+".json";
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

void DesktopConnector::registerView(IView * view)
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

// unregister the view
void DesktopConnector::unregisterView( const QString& viewName ){
    ViewInfo* viewInfo = this->findViewInfo( viewName );
    if ( viewInfo != nullptr ){

        (& viewInfo->refreshTimer)->disconnect();
        m_views.erase( viewName );
    }
}

//    static QTime st;

// schedule a view refresh
qint64 DesktopConnector::refreshView(IView * view)
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

void DesktopConnector::removeStateCallback(const IConnector::CallbackID & /*id*/)
{
    qFatal( "not implemented");
}


Carta::Lib::IRemoteVGView * DesktopConnector::makeRemoteVGView(QString viewName)
{
    return new Carta::Core::SimpleRemoteVGView( this, viewName, this);
}

void DesktopConnector::jsSetStateSlot(const QString & key, const QString & value) {
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

void DesktopConnector::jsSendCommandSlot(const QString & sessionID, const QString &cmd, const QString & parameter)
{
    QString name = QThread::currentThread()->objectName();
    qDebug() << "current thread name:" << name;
    if (name != sessionID) {
        qDebug()<< "ignore jsSendCommandSlot(";
        return;
    }


    if (cmd == "/CartaObjects/DataLoader:getData") {

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        Carta::Data::DataLoader *m_dataLoader = objMan->createObject<Carta::Data::DataLoader>();

        QString fileList = m_dataLoader->getFileList(parameter);

        emit jsCommandResultsSignal(sessionID, cmd, fileList);


    } else if (cmd.contains("ViewManager")) {

        if (cmd == "/CartaObjects/ViewManager:registerView") {
            int kkk = 5;
        }

        QStringList myStringList = cmd.split(':');
        if(myStringList.size()>=2){
              auto subCommand = myStringList[1]; //registerView
              if (subCommand == "registerView") {
                QString result;
                result = this->viewer.m_viewManager->registerView(parameter);
                emit jsCommandResultsSignal(sessionID, cmd, result);
              } else if (subCommand == "dataLoaded") {
                  QString result;
                  result = this->viewer.m_viewManager->dataLoaded(parameter);
                  emit jsCommandResultsSignal(sessionID, cmd, result);
              }
        }

    } else {



        // call all registered callbacks and collect results, but asynchronously
    //    defer( [cmd, parameter, this ]() {
            auto & allCallbacks = m_commandCallbackMap[ cmd];
            QStringList results;
            for( auto & cb : allCallbacks) {
                results += cb( cmd, parameter, "1"); // session id fixed to "1"
            }

            // pass results back to javascript
            emit jsCommandResultsSignal(sessionID, cmd, results.join("|"));

            if( allCallbacks.size() == 0) {
                qWarning() << "JS command has no server listener:" << cmd << parameter;
            }
    //    });
    }
//    /CartaObjects/DataLoader:getData
//    path:

//    qDebug()<<"cmd:"<<cmd;
//    qDebug()<<"parameter:"<< parameter;
//    if (cmd=="/CartaObjects/c14:setZoomLevel") {
//        int kkk =0;
//    }


}

void DesktopConnector::jsConnectorReadySlot()
{
    // at this point it's safe to start using setState as the javascript
    // connector has registered to listen for the signal
    qDebug() << "JS Connector is ready!!!!";

    // time to call the initialize callback
    defer( std::bind( m_initializeCallback, true));

//    m_initializeCallback(true);
}

DesktopConnector::ViewInfo * DesktopConnector::findViewInfo( const QString & viewName)
{
    auto viewIter = m_views.find( viewName);
    if( viewIter == m_views.end()) {
        qWarning() << "DesktopConnector::findViewInfo: Unknown view " << viewName;
        return nullptr;
    }

    return viewIter-> second;
}

void DesktopConnector::refreshViewNow(IView *view)
{

    QString sessionID = QThread::currentThread()->objectName();


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
           emit jsViewUpdatedSignal(sessionID, view-> name(), base64Str, viewInfo-> refreshId);

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
           emit jsViewUpdatedSignal(sessionID, view-> name(), base64Str,  viewInfo-> refreshId);

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
//    auto sendBackToWSClient = [this](const QImage & img) {

    // 1. convert Qimage to jpeg base64 string


    // 2.d send
    //test_pClient->sendTextMessage();

//    };




}

IConnector* DesktopConnector::getConnectorInMap(const QString & sessionID){
    return nullptr;
}

void DesktopConnector::setConnectorInMap(const QString & sessionID, IConnector *connector){
}

void DesktopConnector::startViewerSlot(const QString & sessionID) {

    QString name = QThread::currentThread()->objectName();
    qDebug() << "current thread name:" << name;
    if (name != sessionID) {
        qDebug()<< "ignore startViewerSlot";
        return;
    }

    viewer.start();
}


void DesktopConnector::jsUpdateViewSlot(const QString & sessionID, const QString & viewName, int width, int height)
{
    QString name = QThread::currentThread()->objectName();
    qDebug() << "current thread name:" << name;
    if (name != sessionID) {
        qDebug()<< "ignore jsUpdateViewSlot";
        return;
    }


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

void DesktopConnector::jsViewRefreshedSlot(const QString & viewName, qint64 id)
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

void DesktopConnector::jsMouseMoveSlot(const QString &viewName, int x, int y)
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

void DesktopConnector::stateChangedSlot(const QString & key, const QString & value)
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
