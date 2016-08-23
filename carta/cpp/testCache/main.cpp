//#include "core/Viewer.h"
#include "core/MyQApp.h"
#include "core/CmdLine.h"
#include "core/MainConfig.h"
#include "core/Globals.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/Hooks/Initialize.h"
#include "LevelDbIPCache.h"
#include <QDebug>
#include <QTime>

namespace tCache
{
std::unique_ptr < Carta::Lib::IPCache > pcache;
int imWidth = 20;
int imHeight = 8000;
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

QByteArray vd2qb( const std::vector<double> & vd) {
    QByteArray ba;
    for( const double & d : vd) {
        ba.append( (const char *)( & d), sizeof( double));
    }
    return ba;
}

std::vector<double> qb2vd( const QByteArray & ba) {
    std::vector<double> vd;
    if( ba.size() % sizeof(double) != 0) {
        return vd;
    }
    const char * cptr = ba.constData();
    for( int i = 0 ; i < ba.size() ; i += sizeof(double)) {
        vd.push_back( * ((const double *) (cptr + i)));
    }
    return vd;
}

static std::vector < double >
readProfile( int x, int y) {

    QString key = QString( "%1/%2").arg(x).arg(y);
    QByteArray val;
    if( ! pcache-> readEntry( key.toUtf8(), val))
    {
        return {};
    }
    if( val.size() != int(sizeof(double) * imDepth)) {
        qWarning() << "Cache size mismatch";
        return {};
    }
//    std::vector<double> res(imWidth);
//    const double * dptr = reinterpret_cast<const double*> (val.constData());
//    for( int z = 0 ; z < imDepth ; z ++ ) {
//        res[z] = * dptr;
//        dptr ++;
//    }

    return qb2vd( val);
}

static void
populateCache()
{
    // figure out which row we have already finished generating

    int startx = 0;

    QByteArray buff;
    if ( pcache-> readEntry( "lastx", buff ) ) {
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

//            QByteArray ba = QByteArray::fromRawData(
//                reinterpret_cast < char * > ( & arr[0] ),
//                sizeof( double ) * imDepth );
//            pcache-> setEntry(
//                keyString.toUtf8(), ba, 0 );

            pcache-> setEntry( keyString.toUtf8(), vd2qb(arr), 0);
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

            QByteArray ba;
            if( ! pcache-> readEntry( keyString.toUtf8(), ba)) {
                qCritical() << "Failed to read" << x << y;
            }
            std::vector< double> arr2 = qb2vd(ba);
            if( arr != arr2) {
                qCritical() << "Failed to match" << x << y;

            }

            /*

            QByteArray ba = QByteArray::fromRawData(
                reinterpret_cast < char * > ( & arr[0] ),
                sizeof( double ) * imDepth );

            QString keyString = QString( "%1/%2" ).arg( x ).arg( y );
            QByteArray ba2;
            if( ! pcache-> readEntry( keyString.toUtf8(), ba2)) {
                qCritical() << "Failed to read" << x << y;
            }
            if( ba != ba2) {
                qCritical() << "Failed to match" << x << y << ba.size() << ba2.size();
                qDebug() << * (const double *) (ba.constData());
                qDebug() << * (const double *) (ba2.constData());
            }
            */
        }
        qDebug() << "  " << (x+1) * imHeight * 1000.0 / t.elapsed() << "pix/s";
    }

}

static void
testCache()
{
    pcache.reset(
        new LevelDbIPCache() );

    // populate cache
    populateCache();

    // test cache by reading it
    readCache();

    QByteArray key = "hello";
    QByteArray val;
    if ( pcache-> readEntry( key, val ) ) {
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

    if ( cmdLineInfo.fileList().size() < 2 ) {
        qFatal( "Need 2 files" );
    }

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

    testCache();

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
