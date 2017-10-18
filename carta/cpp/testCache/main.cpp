#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/GetPersistentCache.h"
#include "core/MyQApp.h"
#include "core/CmdLine.h"
#include "core/MainConfig.h"
#include "core/Globals.h"
#include "core/Algorithms/cacheUtils.h"
#include "CartaLib/IPCache.h"
#include <QDebug>
#include <QTime>
#include <QJsonArray>

namespace tCache
{
std::shared_ptr < Carta::Lib::IPCache > pcache;

int imWidth = 20;
int imHeight = 20;
int imDepth = 16000;

double
generateData( int x, int y, int z )
{
    return ( x + 0.1 ) * ( y - 0.1 ) / ( z * x + 0.1 );
}

static std::vector < double >
genProfile( int x, int y )
{
    std::vector < double > arr( imDepth );
    for ( int z = 0 ; z < imDepth ; z++ ) {
        arr[z] = generateData( x, y, z );
    }
    return arr;
}

static std::vector < double >
readProfile( int x, int y )
{
    QString key = QString( "%1/%2" ).arg( x ).arg( y );
    QByteArray val, error;
    if ( ! pcache-> readEntry( key.toUtf8(), val, error ) ) {
        return { };
    }
    if ( val.size() != int (sizeof( double ) * imDepth) ) {
        qWarning() << "Cache size mismatch";
        return { };
    }

    return qb2vd( val );
} // readProfile

static void
populateCache()
{
    // figure out which row we have already finished generating

    int startx = 0;

    QByteArray buff, error;
    if ( pcache-> readEntry( "lastx", buff, error ) ) {
        QString str = buff;
        bool ok;
        int x = str.toInt( & ok );
        if ( ok ) {
            startx = x;
        }
    }

    qDebug() << "Starting from x" << startx;
    for ( int x = startx ; x < imWidth ; x++ ) {
        qDebug() << "Writing x" << x;
        for ( int y = 0 ; y < imHeight ; y++ ) {
            std::vector < double > arr = genProfile( x, y );

            // write the entry
            QString keyString = QString( "%1/%2" ).arg( x ).arg( y );
            pcache-> setEntry( keyString.toUtf8(), vd2qb( arr ), 0 );
        }
        pcache-> setEntry( "lastx", QString::number( x + 1 ).toUtf8(), 0 );
    }
} // populateCache

static void
readCache()
{
    QTime t;
    t.restart();
    qDebug() << "Sequential read...";
    for ( int x = 0 ; x < imWidth ; x++ ) {
        qDebug() << "Testing x" << x;
        for ( int y = 0 ; y < imHeight ; y++ ) {
            std::vector < double > arr = genProfile( x, y );
            QString keyString = QString( "%1/%2" ).arg( x ).arg( y );

            QByteArray ba, error;
            if ( ! pcache-> readEntry( keyString.toUtf8(), ba, error ) ) {
                qCritical() << "Failed to read" << x << y;
                continue;
            }
            std::vector < double > arr2 = qb2vd( ba );
            if ( arr != arr2 ) {
                qCritical() << "Failed to match" << x << y;
            }

        }
        qDebug() << "  " << ( x + 1 ) * imHeight * 1000.0 / t.elapsed() << "pix/s";
    }
} // readCache

static void
testCache()
{

    {
        pcache-> setEntry( "hello", "world", 0 );
        QByteArray val, error;
        if ( pcache-> readEntry( "hello", val, error ) ) {
            qDebug() << "hello=" << val;
        }
        else {
            qDebug() << "hello not found";
        }
    }

    // populate cache
    populateCache();

    // test cache by reading it
    readCache();

    QByteArray key = "hello";
    QByteArray val, error;
    if ( pcache-> readEntry( key, val, error ) ) {
        qDebug() << "db already had value" << val;
    }
    else {
        qDebug() << "db did not have value";
    }
    pcache-> setEntry( key, "hola", 0 );
} // testCache

static int
coreMainCPP( QString platformString, int argc, char * * argv )
{

    MyQApp qapp( argc, argv );

    QString appName = "carta-" + platformString;
#ifndef QT_NO_DEBUG_OUTPUT
    appName += "-verbose";
#endif
    if ( CARTA_RUNTIME_CHECKS ) {
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
    
    // Re-enable all plugins so that all pcache plugins are tested
    mainConfig.insert("disabledPlugins", QJsonValue(QJsonArray()));
     
    // Use test database files
    // Hideous, but apparently the only way to modify nested QJSONObject values.
     
    QJsonObject plugins = mainConfig.json()["plugins"].toObject();
    QJsonObject sqlite3Config = plugins["PCacheSqlite3"].toObject();
    QJsonObject leveldbConfig = plugins["PCacheLevelDB"].toObject();
    sqlite3Config.insert("dbPath", "/tmp/pcache.sqlite.test");
    leveldbConfig.insert("dbPath", "/tmp/pcache.leveldb.test");
    plugins.insert("PCacheSqlite3", QJsonValue(sqlite3Config));
    plugins.insert("PCacheLevelDB", QJsonValue(leveldbConfig));
    mainConfig.insert("plugins", QJsonValue(plugins));
    
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

    // send an initialize hook to all plugins, because some may rely on it
    pm-> prepare < Carta::Lib::Hooks::Initialize > ().executeAll();
    
    
    // make a lambda to set the value of pcache and call the tests
    auto lam = [=] ( const Carta::Lib::Hooks::GetPersistentCache::ResultType &res ) {
        pcache = res;
        pcache->deleteAll();
        testCache();
    };
    
    // call the lambda on every pcache plugin
    auto pcacheRes = pm-> prepare< Carta::Lib::Hooks::GetPersistentCache >();
    pcacheRes.forEach(lam);

    // give QT control
//    int res = qapp.exec();
    int res = 0;

    // if we get here, it means we are quitting...
    qDebug() << "Exiting";
    return res;
} // coreMainCPP
}

int
main( int argc, char * * argv )
{
    try {
        return tCache::coreMainCPP( "tRegion", argc, argv );
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
