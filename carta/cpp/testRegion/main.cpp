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
#include "CartaLib/Regions/CoordinateSystemFormatter.h"
#include <QDebug>
#include <cmath>

namespace tRegion
{

namespace CLR = Carta::Lib::Regions;

static void
testFormatting()
{
    qDebug() << "Test1: pixel coordinate system";
    {
        CLR::CompositeCoordinateSystem cs( 5 );
        CLR::CoordinateSystemFormatter f;

        // get the labels for the axes
        qDebug() << "Labels" << f.getLabels( cs );

        // format a coordinate
        qDebug() << "Coord:" << f.format( cs, { 1.0, 2.0, 3.0, 1.0 }
                                          );
    }
    qDebug() << "Test2: spectral cube system";

    // make 4 axis coordinate system
    CLR::CompositeCoordinateSystem cs( 4 );

    // first axis will be galactic longitude
    cs.setAxis( 0, CLR::BasicCoordinateSystemInfo::galactic(), 0 );

    // second axis will be galactic lattitude
    cs.setAxis( 1, CLR::BasicCoordinateSystemInfo::galactic(), 1 );

    // third axis will be frequency
    cs.setAxis( 2, CLR::BasicCoordinateSystemInfo::frequency() );

    // last axis will be stokes
    cs.setAxis( 3, CLR::BasicCoordinateSystemInfo::stokes() );

    // make a formatter
    CLR::CoordinateSystemFormatter f;

    // get the labels for the axes
    qDebug() << "Labels" << f.getLabels( cs );

    // format a coordinate
    qDebug() << "Coord:" << f.format( cs, { 1.0, 2.0, 3.0, 1.0 }
                                      );

} // apiTestFormatting


typedef Carta::Lib::Regions::RegionSet RegionSet;
typedef Carta::Lib::Image::ImageInterface Image;
typedef Carta::Lib::Regions::ICoordSystemConverter ICoordSystemConverter;

RegionSet *
readRegionSet( QString /*fname*/ )
{
    /// \todo implement this
    CLR::Circle * c1 = new CLR::Circle( { 0, 0 }, 30 );

    CLR::Circle * c2 = new CLR::Circle( { 7, 1.5 }, 3.123 );

    qDebug() << "c1 bb=" << c1->outlineBox();
    qDebug() << "c2 bb=" << c2->outlineBox();

    CLR::Union * u1 = new CLR::Union();
    u1->addChild( c1 );
    u1->addChild( c2 );
    c2->setLineColor( "blue" );

    RegionSet * rSet = new RegionSet();
    rSet-> setRoot( c1);

    auto cvt = std::make_shared<CLR::PixelIdentityCSConverter>( 2);
    rSet-> addCoordSystemConverter( cvt);

    return rSet;
} // readRegionSet

/// makes a converter between two composite coordinate systems
Carta::Lib::Regions::ICoordSystemConverter *
makeConverter( const CLR::CompositeCoordinateSystem & src,
               const CLR::CompositeCoordinateSystem & dst )
{
    /// \todo implement this properly...
    int ndim = std::min( src.ndim(), dst.ndim() );

    return new Carta::Lib::Regions::PixelIdentityCSConverter( ndim );
}

static void
testRegionOnImage( QString imageFname, QString regionFname )
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

    // get the coordinate system from img
    auto imgMeta = astroImage-> metaData();
    ICoordSystemConverter::SharedPtr imgCS = imgMeta-> getCSConv();
    Carta::Lib::Regions::CoordinateSystemFormatter fmt;
    qDebug() << "Image CS labels:" << fmt.getLabels( imgCS->srcCS() );
    qDebug() << "Image CS labels:" << fmt.getLabels( imgCS->dstCS() );

    const auto & imageWCS = imgCS-> dstCS();

    // get the region set from fname1
    RegionSet * rSet = readRegionSet( regionFname );
    CARTA_ASSERT( rSet );

    // create an array of converters for each coordinate system in the region set

    // go through all pixels in the image and test whether they are inside region,
    // calculating some statistics
    struct {
        int64_t nPixels = 0;
        int64_t nPixelsReal = 0;
        double sum = 0.0;
        double min = 0.0;
        double max = 0.0;
    }
    stats;

    // this will be the coordinate of the pixel we are currently processing
    // in integers
    std::vector < int > iPos {
        0, 0
    };

    // this will be the coordinate of the pixel we are currently processing
    // in doubles
    std::vector < double > fPos {
        0, 0
    };

    std::vector < double > fPos2 {
        0, 0
    };

    std::vector < double > fPos3 {
        0, 0
    };

    // make a buffer for points converted to the region coordinate systems
    auto nRegionCS = rSet->nCoordSystems();
    std::vector < Carta::Lib::Regions::RegionPoint > cvPoints( nRegionCS );

    // create the converters
    std::vector < CLR::ICoordSystemConverter::UniquePtr > converters( nRegionCS );
    for ( int i = 0 ; i < nRegionCS ; i++ ) {
        converters[i].reset( makeConverter( imageWCS, rSet->coordSystemConverter( i ).dstCS() ) );
    }

    int width = astroImage->dims()[0];
    int height = astroImage->dims()[1];

    // make a double view of the data
    auto dataSliceRaw = astroImage->getDataSlice( SliceND() );
    auto dataSlice = Carta::Lib::NdArray::TypedView < double > ( dataSliceRaw, true );


    // iterate through the data one pixel at a time
    for ( double y = 0 ; y < height ; y++ ) {
        qDebug() << "y=" << y;
        fPos[1] = y;
        iPos[1] = y;
        for ( double x = 0 ; x < width ; x++ ) {
            fPos[0] = x;
            iPos[0] = x;

            // convert fPos to region coordinate systems
            for ( int i = 0 ; i < nRegionCS ; i++ ) {
                // first convert from image WCS to region's WCS
                converters[i]-> src2dst( fPos, fPos2 );
                // convert from region's WCS to region's pixel CS
                rSet->coordSystemConverter(i).dst2src( fPos2, fPos3);
                // convert to QPointF
                cvPoints[i] = QPointF( fPos2[0], fPos2[1] );
            }

            // check if the point is inside
            if ( rSet-> isPointInside( cvPoints ) ) {
                double pixel = dataSlice.get( iPos );
                stats.nPixels++;
                if ( ! std::isnan( pixel ) ) {
                    stats.nPixelsReal++;
                    stats.sum += pixel;
                }
            }
        }
    }

    // report some results:
    qDebug() << "#pixels in region:" << stats.nPixelsReal;
    qDebug() << "#non-nan pixels in region:" << stats.nPixelsReal;
    qDebug() << "sum:" << stats.sum;
}

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


    testFormatting();

    testRegionOnImage( cmdLineInfo.fileList()[0], cmdLineInfo.fileList()[1] );

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
