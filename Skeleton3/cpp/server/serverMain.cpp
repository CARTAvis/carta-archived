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

    qDebug() << "Starting" << qapp.applicationName() << qapp.applicationVersion();
    qDebug() << "Command line main";
    // alias globals
    auto & globals = * Globals::instance();
    qDebug() << "Got globals";
    // parse command line arguments & environment variables
    auto cmdLineInfo = CmdLine::parse( MyQApp::arguments());
    globals.setCmdLineInfo( & cmdLineInfo);
    qDebug() << "Got command line info";
    // load the config file
    QString configFilePath = cmdLineInfo.configFilePath();
    auto mainConfig = MainConfig::parse( configFilePath);
    globals.setMainConfig( & mainConfig);
    qDebug() << "plugin directories:\n - " + mainConfig.pluginDirectories().join( "\n - ");

    // initialize platform
    // platform gets command line & main config file via globals ?!?!?
    auto platform = new ServerPlatform();
    globals.setPlatform( platform);
    qDebug()<<"Made server platform";
    // prepare connector
    IConnector * connector = platform-> connector();
    if( ! connector) {
        qFatal( "Could not initialize connector!");
    }
    globals.setConnector( connector);
    qDebug() << "Set connector";
    // create the viewer
    Viewer viewer;
    qDebug() << "Make viewer";
    // prepare closure to execute when connector is initialized
    IConnector::InitializeCallback initCB = [connector, & viewer](bool valid) -> void {
        qDebug() << "Connector initialized:" << valid;
        viewer.start();
    };

    // initialize connector
    connector-> initialize( initCB);

    // qt now has control
    return qapp.exec();

}
