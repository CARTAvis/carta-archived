/**
 *
 **/

#include "DesktopConnector.h"
//#include "ViewInfoPrivate.h"
#include "common/misc.h"
#include "common/LinearMap.h"
#include <iostream>
#include <QImage>
#include <QPainter>
#include <cmath>
#include <QTime>
#include <QTimer>
#include <QCoreApplication>

///
/// \brief internal class of DesktopConnector, containing extra information we like
///  to remember with each view
///
struct DesktopConnector::ViewInfo
{

    /// the implemented IView
    IView * view;

    /// last received client size
    QSize clientSize;
    // linear maps convert x,y from client to image coordinates
    LinearMap1D tx, ty;

    // refresh timer for this object
    QTimer refreshTimer;

    ViewInfo( IView * pview )
    {
        view = pview;
        clientSize = QSize(1,1);
        refreshTimer.setSingleShot( true);
        // just long enough that two successive calls will result in only one redraw :)
        refreshTimer.setInterval( 1);
    }

};

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
    // find the list of callbacks for this path
    auto iter = m_stateCallbackList.find( path);

    // if it does not exist, create it
    if( iter == m_stateCallbackList.end()) {
//        qDebug() << "Creating callback list for variable " << path;
        auto res = m_stateCallbackList.insert( std::make_pair(path, new StateCBList));
        iter = res.first;
    }

    iter = m_stateCallbackList.find( path);
    if( iter == m_stateCallbackList.end()) {
//        qDebug() << "What the hell";
    }

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
    connect( & viewInfo->refreshTimer, & QTimer::timeout, [=] () {
        refreshViewNow( view);
    });
}

//    static QTime st;

// schedule a view refresh
void DesktopConnector::refreshView(IView * view)
{
    // find the corresponding view info
    ViewInfo * viewInfo = findViewInfo( view-> name());
    if( ! viewInfo) {
        // this is an internal error...
        std::cerr << "Critical: refreshView cannot find this view: " << view-> name() << "\n";
        return;
    }

    // start the timer for this view if it's not already started
    if( ! viewInfo-> refreshTimer.isActive()) {
        viewInfo-> refreshTimer.start();
        std::cerr << "Scheduled refresh for " << view->name() << "\n";
    }
    else {
        std::cerr << "########### saved refresh for " << view->name() << "\n";
    }

//    QCoreApplication::postEvent( this, new ViewRefreshEvent( getRefreshViewEventType(), view), Qt::NormalEventPriority);
    //    refreshViewNow( view);
}

void DesktopConnector::removeStateCallback(const IConnector::CallbackID & /*id*/)
{
    qFatal( "not implemented");
}

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

DesktopConnector::ViewInfo * DesktopConnector::findViewInfo( const QString & viewName)
{
    auto viewIter = m_views.find( viewName);
    if( viewIter == m_views.end()) {
        std::cerr << "Unknown view " << viewName << "\n";
        return nullptr;
    }

    return viewIter-> second;
}

void DesktopConnector::refreshViewNow(IView *view)
{

    std::cerr << "refreshViewNow " << view->name() << "\n";

    //    std::cerr << "timer " << st.elapsed() << "\n";
    //    st.restart();

    ViewInfo * viewInfo = findViewInfo( view-> name());
    if( ! viewInfo) {
        // this is an internal error...
        std::cerr << "Critical: refreshView cannot find this view: " << view-> name() << "\n";
        return;
    }
    // get the image from view
    const QImage & origImage = view-> getBuffer();
    // scale the image to fit the client size
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
    viewInfo-> tx = LinearMap1D( xOffset, xOffset + destImage.size().width()-1,
                                 0, origImage.width()-1);
    viewInfo-> ty = LinearMap1D( yOffset, yOffset + destImage.size().height()-1,
                                 0, origImage.height()-1);

    emit jsViewUpdatedSignal( view-> name(), pix);
}

void DesktopConnector::jsUpdateViewSlot(const QString &viewName, int width, int height)
{
    ViewInfo * viewInfo = findViewInfo( viewName);
    if( ! viewInfo) {
        std::cerr << "Received update for unknown view " << viewName << "\n";
        return;
    }

    IView * view = viewInfo-> view;
    viewInfo->clientSize = QSize( width, height);
    view-> handleResizeRequest( QSize( width, height));
    refreshView( view);
}

void DesktopConnector::jsMouseMoveSlot(const QString &viewName, int x, int y)
{
    ViewInfo * viewInfo = findViewInfo( viewName);
    if( ! viewInfo) {
        std::cerr << "Received mouse event for unknown view " << viewName << "\n";
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
//    QTime t; t.restart();
    view-> handleMouseEvent( ev);
//    std::cerr << "Mouse event hanled in " << t.elapsed() << "ms\n";
}

void DesktopConnector::stateChangedSlot(const QString & key, const QString & value)
{
//    qDebug() << "state changed slot " << key << " = " << value;

    // find the list of callbacks for this path
    auto iter = m_stateCallbackList.find( key);

    // if it does not exist, do nothing
    if( iter == m_stateCallbackList.end()) {
//        qDebug() << "no callbacks registered for" << key;
        return;
    }

//    qDebug() << "calling all registered callbacks for" << key;

    iter-> second-> callEveryone( key, value);

//            m_stateCallbackList[ key].callEveryone( key, value);

    //    // we just call our own callbacks here
//    for( auto & cb : m_stateCallbackList[ key]) {
//        cb( key, value);
//    }
}
