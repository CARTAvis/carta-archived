/*
 * This is the client-server main
 */

#include "ServerPlatform.h"
#include "common/Viewer.h"
#include "common/HackViewer.h"
#include "common/MyQApp.h"
#include "common/CmdLine.h"
#include "common/MainConfig.h"
#include "common/Globals.h"
#include <QDebug>

///
/// \brief main entry point for server viewer
/// \param argc standard argc
/// \param argv standard argv
/// \return standard main return (0=success)
///
/// @todo refactor some of the common code in desktopMain and serverMain into a commonMain
///
/// @warning keep this in sync with serverMain until refactored to commonMain
///
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

    // alias globals
    auto & globals = * Globals::instance();

    // parse command line arguments & environment variables
    // ====================================================
    auto cmdLineInfo = CmdLine::parse( MyQApp::arguments());
    globals.setCmdLineInfo( & cmdLineInfo);

    // load the config file
    // ====================
    QString configFilePath = cmdLineInfo.configFilePath();
    auto mainConfig = MainConfig::parse( configFilePath);
    globals.setMainConfig( & mainConfig);
    qDebug() << "plugin directories:\n - " + mainConfig.pluginDirectories().join( "\n - ");

    // initialize platform
    // ===================
    // platform gets command line & main config file via globals
    auto platform = new ServerPlatform();
    globals.setPlatform( platform);

    // prepare connector
    // =================
    // connector is created via platform, but we put it into globals explicitely here
    IConnector * connector = platform-> connector();
    if( ! connector) {
        qFatal( "Could not initialize connector!");
    }
    globals.setConnector( connector);

    // initialize plugin manager
    // =========================
    globals.setPluginManager( new PluginManager );
    auto pm = globals.pluginManager();
    // tell plugin manager where to find plugins
    pm-> setPluginSearchPaths( globals.mainConfig()->pluginDirectories() );
    // find and load plugins
    pm-> loadPlugins();
    qDebug() << "Loading plugins...";
    auto infoList = pm-> getInfoList();
    qDebug() << "List of plugins: [" << infoList.size() << "]";
    for ( const auto & entry : infoList ) {
        qDebug() << "  path:" << entry.json.name;
    }

    // create the viewer
    // =================
    Viewer viewer;
    HackViewer hackViewer;
    qDebug() << "Make viewer";
    // prepare closure to execute when connector is initialized
    IConnector::InitializeCallback initCB = [&](bool valid) -> void {
        if( ! valid) {
            qFatal( "Could not initialize connector");
        }
        viewer.start();
        hackViewer.start();
    };

    // initialize connector
    connector-> initialize( initCB);

    // qt now has control
    return qapp.exec();
}
