//#include "DesktopPlatform.h"

/*
 * This is the 'main' that is shared between both server and desktop versions.
 */

//#include "core/Viewer.h"
#include "core/MyQApp.h"
#include "core/CmdLine.h"
#include "core/MainConfig.h"
#include "core/Globals.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Regions/IRegion.h"
#include <QDebug>

namespace tRegion
{
typedef Carta::Lib::Regions::RegionSet RegionSet;
typedef Carta::Lib::Image::ImageInterface Image;
typedef Carta::Lib::Regions::ICoordSystem ICoordSystem;

RegionSet *
readRegionSet( QString /*fname*/ )
{
    return nullptr;
}

Image *
readImage( QString /*fname*/ )
{
    return nullptr;
}

static void
runTest1( QString imageFname, QString regionFname )
{
    auto pm = Globals::instance()->pluginManager();

    // tell all plugins that the core has initialized
    pm-> prepare < Carta::Lib::Hooks::Initialize > ().executeAll();

    auto imgRes = pm-> prepare < Carta::Lib::Hooks::LoadAstroImage > ( imageFname ).first();
    if ( imgRes.isNull() ) {
        throw "Could not find any plugin to load astroImage";
    }

    Carta::Lib::Image::ImageInterface::SharedPtr astroImage = imgRes.val();
    if ( ! astroImage ) {
        throw "Image read was a nullptr";
    }

    qDebug() << "Loaded image has dimensions: " << astroImage-> dims();

    // get the region set from fname1
    RegionSet * rSet = readRegionSet( regionFname );
    CARTA_ASSERT( rSet);

    // get the coordinate system from img
    auto imgMeta = astroImage-> metaData();
    ICoordSystem * imgCS = imgMeta-> getCS();

    // set the destination coordinate system for the region set
    rSet-> setInputCS( imgCS );

    // go through all pixels in the image and test whether they are inside region,
    // calculating some statistics
    struct {
        int nPixels = 0;
        double sum = 0.0;
        double min = 0.0;
        double max = 0.0;
    }
    stats;

    std::vector < double > coord {
        0, 0
    };
    std::vector < int > pos {
        0, 0
    };
    int width = astroImage->dims()[0];
    int height = astroImage->dims()[1];

    auto dataSliceRaw = astroImage->getDataSlice( SliceND() );
    auto dataSlice = Carta::Lib::NdArray::TypedView < double > ( dataSliceRaw, true );

    for ( double y = 0 ; y < height ; y++ ) {
        pos[1] = y;
        coord[1] = y;
        for ( double x = 0 ; x < width ; x++ ) {
            pos[0] = x;
            coord[0] = x;
            if ( rSet-> isPointInside( coord ) ) {
                double pixel = dataSlice.get( pos );
                if ( std::isnan( pixel ) ) {
                    stats.nPixels++;
                    stats.sum += pixel;
                }
            }
        }
    }
} // apiTest2

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

    runTest1( cmdLineInfo.fileList()[0], cmdLineInfo.fileList()[1] );

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
        return tRegion::coreMainCPP( "tRegion", argc, argv );
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
