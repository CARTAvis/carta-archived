/*
 * This is the desktop main
 */

#include "DesktopPlatform.h"
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
    MyQApp::setApplicationName( "Skeleton3-desktop-debug");
#else
    MyQApp::setApplicationName( "Skeleton3-desktop-release");
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
    // platform needs command line & global settings
    auto platform = new DesktopPlatform();

    qDebug() << "Starting" << qapp.applicationName() << qapp.applicationVersion();
    qDebug() << "Application path:" << qapp.applicationDirPath();

    // create the viewer with this platform
    Viewer viewer( platform);

    // run the viewer
    viewer.start();
    return qapp.exec();
}

// TODO: MyQApp, command line parsing, config file parsing and Platform are all
// boiler plate code. All these should be probably into single code, maybe even
// into Platform. The problem with this right now is that DesktopPlatform is
// a QObject, and Qt for some reason chokes on instantiating QObject derived
// classes before QApplication is initialized... The reason DesktopPlatform is
// a QObject derived class is that we use it in webkit to give javascript direct
// access to platform commands, which at the moment is only the fullscreen api...
