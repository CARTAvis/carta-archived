/*
 * This is the client-server main
 */

#include "ServerPlatform.h"
#include "common/Viewer.h"
#include "common/MyQApp.h"
#include <QDebug>

int main(int argc, char ** argv)
{
    qDebug() << "Server app is starting up.";
#ifdef QT_DEBUG
  qDebug() << "Running a debug build";
#else
  qDebug() << "Running a release build";
#endif

    MyQApp qapp( argc, argv);

    qDebug() << "Command line args: " << MyQApp::arguments();

    // initialize platform
    ServerPlatform * platform = new ServerPlatform();

    // create viewer with this platform
    Viewer viewer( platform);

    // run the viewer
    viewer.start();
    return qapp.exec();
}
