/**
 *
 **/

#include "DesktopConnector.h"
#include "common/misc.h"
#include "common/LinearMap.h"
#include <iostream>
#include <QImage>
#include <QPainter>
#include <cmath>

///
/// \brief Internal class, containing extra information we like to remember per view.
///
struct DesktopConnector::ViewInfo {
    /// the real view
    IView * view;
    /// client size
    QSize clientSize;
    LinearMap1D tx, ty; // linear maps convert x,y from client to image coordinates

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
    m_stateCallbackList[ path].push_back( cb);
    return 1;
}

void DesktopConnector::registerView(IView * view)
{
    // let the view know it's registered, and give it access to the connector
    view->registration( this);

    // insert this view int our list of views
    ViewInfo * viewInfo = new ViewInfo;
    viewInfo-> view = view;
    viewInfo-> clientSize = QSize(1,1);
    m_views[ view-> name()] = viewInfo;
}

void DesktopConnector::refreshView(IView * view)
{
    ViewInfo * viewInfo = findViewInfo( view-> name());
    if( ! viewInfo) {
        // this is an internal error...
        std::cerr << "Critical: refreshView cannot find this view: " << view-> name() << "\n";
        return;
    }
    // get the image from view
    const QImage & origImage = view-> getBuffer();
    // scale the image to fit the client size
    QImage destImage = origImage.scaled( viewInfo->clientSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
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

    std::cerr << "mm " << x << "," << y << " -> " << xi << "," << yi << "\n";

    // tell the view about the event
    QMouseEvent ev( QEvent::MouseMove,
                    QPoint(xi,yi),
                    Qt::NoButton,
                    Qt::NoButton,
                    Qt::NoModifier   );
    view-> handleMouseEvent( ev);
}

void DesktopConnector::stateChangedSlot(const QString &key, const QString &value)
{
    // we just call our own callbacks here
    for( auto & cb : m_stateCallbackList[ key]) {
        cb( key, value);
    }
}
