#pragma once

/*
 * This is the 'main' that is shared between both server and desktop versions.
 */

#include "core/Viewer.h"
#include "core/Hacks/HackViewer.h"
#include "core/MyQApp.h"
#include "core/CmdLine.h"
#include "core/MainConfig.h"
#include "core/Globals.h"
#include <QDebug>

namespace Carta
{
namespace Core
{
///
/// \brief internal main entry point for the viewer
/// \param argc standard argc
/// \param argv standard argv
/// \return standard main return (0=success)
///
template < class Platform >
static int
coreMainCPP( QString platformString, int argc, char * * argv )
{
    //
    // initialize Qt
    //
    // don't require a window manager even though we're a QGuiApplication
//    qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("minimal"));

    MyQApp qapp( argc, argv );

    QString appName = "carta-" + platformString;
#ifndef QT_NO_DEBUG_OUTPUT
    appName += "-verbose";
#endif
    if( CARTA_RUNTIME_CHECKS) {
        appName += "-runtimeChecks";
    }
    MyQApp::setApplicationName( appName );

    qDebug() << "Starting" << qapp.applicationName() << qapp.applicationVersion();

    // alias globals
    auto & globals = * Globals::instance();

    // parse command line arguments & environment variables
    // ====================================================
    auto cmdLineInfo = CmdLine::parse( MyQApp::arguments() );
    globals.setCmdLineInfo( & cmdLineInfo );

    // load the config file
    // ====================
    QString configFilePath = cmdLineInfo.configFilePath();
    MainConfig::ParsedInfo mainConfig = MainConfig::parse( configFilePath );
    globals.setMainConfig( & mainConfig );
    qDebug() << "plugin directories:\n - " + mainConfig.pluginDirectories().join( "\n - " );

    // initialize plugin manager
    // =========================
    globals.setPluginManager( std::make_shared < PluginManager > () );
    auto pm = globals.pluginManager();
    // tell plugin manager where to find plugins
    pm-> setPluginSearchPaths( globals.mainConfig()->pluginDirectories() );
    // find and load plugins
    pm-> loadPlugins();
    qDebug() << "Loading plugins...";
    auto infoList = pm-> getInfoList();
    qDebug() << "List of loaded plugins: [" << infoList.size() << "]";
    for ( const auto & entry : infoList ) {
        qDebug() << "  path:" << entry.json.name;
    }

    // initialize platform
    // ===================
    // platform get access to
    // - command line
    // - main config file
    // - plugin manager
    // via Globals::instance()
    globals.setPlatform( new Platform() );

    // prepare connector
    // =================
    // connector is created via platform, but we put it into globals explicitely here
    IConnector * connector = globals.platform()-> connector();
    if ( ! connector ) {
        qFatal( "Could not initialize connector!" );
    }
    globals.setConnector( connector );

    // create the viewer
    // =================
    Viewer viewer;
    Hacks::HackViewer::UniquePtr hackViewer = nullptr;
    if ( globals.mainConfig()-> hacksEnabled() ) {
        hackViewer.reset( new Hacks::HackViewer );
    }
    if ( globals.mainConfig()->isDeveloperLayout() ) {
        viewer.setDeveloperView();
    }

    // prepare closure to execute when connector is initialized
    IConnector::InitializeCallback initCB = [&] ( bool valid ) -> void {
        if ( ! valid ) {
            qFatal( "Could not initialize connector" );
        }
        viewer.start();
        if ( hackViewer ) {
            hackViewer-> start();
        }
    };

    // initialize connector
    connector-> initialize( initCB );

    // give QT control
    int res = qapp.exec();

    // if we get here, it means we are quitting...
    qDebug() << "Exiting";
    return res;
} // mainCPP

/// main entry point, wrapped in try/catch to report exceptions during startup
template < class Platform >
int
coreMain( QString platformString, int argc, char * * argv )
{
    try {
        return coreMainCPP<Platform>( platformString, argc, argv );
    }
    catch ( const char * err ) {
        qCritical() << "Exception(char*):" << err;
    }
    catch ( const std::string & err ) {
        qCritical() << "Exception(std::string &):" << err.c_str();
    }
    catch ( const QString & err ) {
        qCritical() << "Exception(QString &):" << err;
    }
    catch ( ... ) {
        qCritical() << "Exception(unknown type)!";
    }
    qFatal( "%s", "...caught in main()" );
    return - 1;
} // main
}
}
