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
    // initialize Qt
    MyQApp qapp( argc, argv);

#ifdef QT_DEBUG
    MyQApp::setApplicationName( "Skeleton3-desktop-debug");
#else
    MyQApp::setApplicationName( "Skeleton3-desktop-release");
#endif

    qDebug() << "Starting" << qapp.applicationName() << qapp.applicationVersion();

    // alias globals
    auto & globals = * Globals::instance();

    // parse command line arguments & environment variables
    auto cmdLineInfo = CmdLine::parse( MyQApp::arguments());
    globals.setCmdLineInfo( & cmdLineInfo);

    // load the config file
    QString configFilePath = cmdLineInfo.configFilePath();
    auto mainConfig = MainConfig::parse( configFilePath);
    globals.setMainConfig( & mainConfig);
    qDebug() << "plugin directories:\n - " + mainConfig.pluginDirectories().join( "\n - ");

    // initialize platform
    // platform gets command line & main config file via globals ?!?!?
    auto platform = new DesktopPlatform();
    globals.setPlatform( platform);

    // prepare connector
    IConnector * connector = platform-> connector();
    globals.setConnector( connector);

    // create the viewer
    Viewer viewer;

    // prepare closure to execute when connector is initialized
    IConnector::InitializeCallback initCB = [connector, & viewer](bool valid) -> void {
        qDebug() << "Connector initialized:" << valid;
        viewer.start();
    };

    // initialize connector
    if( ! connector || ! connector-> initialize( initCB)) {
        qFatal( "Could not initialize connector!");
    }

    // qt now has control
    return qapp.exec();
}

// TODO: MyQApp, command line parsing, config file parsing and Platform are all
// boiler plate code. All these should be probably into single code, maybe even
// into Platform. The problem with this right now is that DesktopPlatform is
// a QObject, and Qt for some reason chokes on instantiating QObject derived
// classes before QApplication is initialized... The reason DesktopPlatform is
// a QObject derived class is that we use it in webkit to give javascript direct
// access to platform commands, which at the moment is only the fullscreen api...
