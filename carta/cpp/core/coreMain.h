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
#include <QDir>
#include <QTime>

// custmoize the style of print out messages
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QDateTime dateTime(QDateTime::currentDateTime());
    QString timeStr(dateTime.toString("dd-MM-yyyy HH:mm:ss"));

    // set the output of log file "carta.log" at the user's home directory
    QFile outFile(QDir::homePath() + "/carta.log");

    // set the size of output log file "carta.log" is 5 MB.
    // if log file is excess the limitation, remove it and reload the new message.
    int setFileSizeLimit = 5000000; // unit: bytes (1K = 1000B, 1M = 1000K)
    if (outFile.size() > setFileSizeLimit) {
        outFile.remove();
    }

    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream stream(&outFile);

    QByteArray localMsg = msg.toLocal8Bit();
    QString criticalMsg = localMsg.constData();

    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", localMsg.constData());
        break;
    //case QtInfoMsg: // it is interduced after Qt 5.5
    //    fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    //    break;
    case QtWarningMsg:
        fprintf(stderr, "\033[1m\033[36mWarning: %s (%s:%u, %s)\033[0m\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
         if (criticalMsg[0] == '<' && criticalMsg[1] == '>') {
            fprintf(stderr, "\033[1m\033[34mInfo: %s \033[0m\n", localMsg.constData());
            stream << timeStr << " " << localMsg.constData() << endl;
        } else {
            fprintf(stderr, "\033[1m\033[35mCritical: %s (%s:%u, %s)\033[0m\n", localMsg.constData(), context.file, context.line, context.function);
        }
        break;
    case QtFatalMsg:
        fprintf(stderr, "\033[1m\033[31mFatal: %s (%s:%u, %s)\033[0m\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

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

    QObject::connect( &qapp, SIGNAL(aboutToQuit()), &viewer, SLOT(DBClose()));

    if (CARTA_RUNTIME_CHECKS) {
        // custmoize the style of print out messages
        qInstallMessageHandler(myMessageOutput);
    }

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
    catch( const std::exception & ex ) {
        qCritical() << "Exception(std::runtime_error)!" << ex.what();
    }
    catch ( ... ) {
        qCritical() << "Exception(unknown type)!";
    }
    qFatal( "%s", "...caught in main()" );
    return - 1;
} // main
}
}
