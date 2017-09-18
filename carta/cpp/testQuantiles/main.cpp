#include <QtTest/QtTest>
#include "core/MyQApp.h"
#include "core/CmdLine.h"
#include "core/MainConfig.h"
#include "core/Globals.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/IImage.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/Hooks/Initialize.h"
#include "core/Algorithms/percentileAlgorithms.h"
#include "core/Algorithms/percentileManku99.h"

class TestQuantiles: public QObject
{
    Q_OBJECT
private:
    Carta::Lib::NdArray::Double doubleView;
    const std::vector<double> percentiles = {0, 0.01, 0.1, 0.9, 0.99, 1};
private slots:
    void initTestCase();
    void exact();
    void manku99();
    void cleanupTestCase();
};


void TestQuantiles::initTestCase()
{
    MyQApp qapp( argc, argv );
    MyQApp::setApplicationName( "test" );

    auto & globals = * Globals::instance();
    auto cmdLineInfo = CmdLine::parse( MyQApp::arguments() );
    globals.setCmdLineInfo( & cmdLineInfo );

    if ( cmdLineInfo.fileList().size() < 1 ) {
        qFatal( "This test needs a commandline parameter: path to a test image." );
    }

    MainConfig::ParsedInfo mainConfig = MainConfig::parse( cmdLineInfo.configFilePath() );
    globals.setMainConfig( & mainConfig );

    globals.setPluginManager( std::make_shared < PluginManager > () );
    auto pm = globals.pluginManager();
    pm-> setPluginSearchPaths( globals.mainConfig()->pluginDirectories() );
    pm-> loadPlugins();
    pm-> prepare < Carta::Lib::Hooks::Initialize > ().executeAll();

    auto imgRes = pm-> prepare < Carta::Lib::Hooks::LoadAstroImage > ( cmdLineInfo.fileList()[0] ).first();
    Carta::Lib::Image::ImageInterface::SharedPtr astroImage = imgRes.val();

    qDebug() << "Loaded image has dimensions: " << astroImage-> dims();

    auto rawView = astroImage->getDataSlice(SliceND());
    doubleView = Carta::Lib::NdArray::Double(rawView, false);
}

void TestQuantiles::exact()
{
    Carta::Core::Algorithms::percentile2pixels(doubleView, percentiles);
}


void TestQuantiles::manku99()
{
    Carta::Core::Algorithms::percentile2pixels_approximate_manku99(doubleView, percentiles);
}


void TestQuantiles::cleanupTestCase()
{
}


QTEST_MAIN(TestQuantiles)
#include "main.moc"
