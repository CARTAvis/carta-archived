/**
 *
 **/


#include "DesktopConnector.h"



DesktopConnector::DesktopConnector( MainWindow *)
{

}

bool DesktopConnector::initialize()
{
    return true;
}

void DesktopConnector::setState(const QString & /*path*/, const QString & /*value*/)
{

}

QString DesktopConnector::getState(const QString & path)
{
    return path;
}

IConnector::CallbackID DesktopConnector::addCommandCallback(
        const QString &/*cmd*/,
        const IConnector::CommandCallback &/*cb*/)
{
    return 1;
}

IConnector::CallbackID DesktopConnector::addStateCallback(
        IConnector::CSR /*path*/,
        const IConnector::StateChangedCallback & /*cb*/)
{
    return 1;
}

void DesktopConnector::registerView(IView *view)
{
    view-> registration( this);
}

void DesktopConnector::refreshView(IView */*view*/)
{

}
