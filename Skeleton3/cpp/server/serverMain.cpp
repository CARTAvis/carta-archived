/*
 * This is the client-server main
 */

#include "ServerPlatform.h"
#include "common/Viewer.h"
#include "common/MyQApp.h"
#include "common/CmdLine.h"
#include "common/MainConfig.h"
#include "common/Globals.h"
#include <QDebug>

int main(int argc, char ** argv)
{
    //
    // initialize Qt
    //
    MyQApp qapp( argc, argv);
#ifdef QT_DEBUG
    MyQApp::setApplicationName( "Skeleton3-server-debug");
#else
    MyQApp::setApplicationName( "Skeleton3-server-release");
#endif

    // parse command line arguments & environment variables
    auto cmdLineInfo = CmdLine::parse( MyQApp::arguments());
    Globals::instance()-> setCmdLineInfo( & cmdLineInfo);

    // load the config file
    QString configFilePath = cmdLineInfo.configFilePath();
    auto mainConfig = MainConfig::parse( configFilePath);
    Globals::instance()-> setMainConfig( & mainConfig);
    qDebug() << "plugin directories:\n - " + mainConfig.pluginDirectories().join( "\n - ");

    // initialize platform
    ServerPlatform * platform = new ServerPlatform();

    qDebug() << "Starting" << qapp.applicationName() << qapp.applicationVersion();
    qDebug() << "Application path:" << qapp.applicationDirPath();

    // create viewer with this platform
    Viewer viewer( platform);

    // run the viewer
    viewer.start();
    return qapp.exec();
}
