/*
 * This is the test for percentile algorithms
 *
 * Usage: $./testPercentile [the path and name of the image file] ...
 *
 * for example: $./testPercentile /home/username/CARTA/Images/cartaImageLibrary/aH.fits ...
 *
 */

#include "core/MyQApp.h"
#include "core/CmdLine.h"
#include "core/MainConfig.h"
#include "core/Globals.h"

// disable the qDebug() function (you can turn on by commenting the following four code lines)
#ifdef qDebug
#undef qDebug
#endif
#define qDebug QT_NO_QDEBUG_MACRO

// we want to check percentile functions from this code
#include "core/Algorithms/percentileAlgorithms.h"

#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/Hooks/Initialize.h"

// get unit conversion headers
#include "CartaLib/Hooks/ConversionSpectralHook.h"
#include "CartaLib/Hooks/ConversionIntensityHook.h"
#include "../plugins/ConversionIntensity/ConverterIntensity.h"

#include "CartaLib/Regions/CoordinateSystemFormatter.h"
#include <QDebug>
#include <cmath>

const std::vector<double> percentile = {0.0005, 0.0025, 0.005, 0.01, 0.015,
                                        0.02, 0.025, 0.0375, 0.05, 0.95,
                                        0.9625, 0.975, 0.98, 0.985, 0.99,
                                        0.995, 0.9975, 0.9995};

const std::vector<double> bin_number = {1000, 10000, 100000, 1000000};

namespace tPercentile {

typedef Carta::Lib::AxisInfo AxisInfo;

int getAxisIndex(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                 Carta::Lib::AxisInfo::KnownType axisType) {
    int index = -1;
    if (m_image){
        std::shared_ptr<CoordinateFormatterInterface> cf(m_image->metaData()->coordinateFormatter()->clone());
        int axisCount = cf->nAxes();
        for ( int i = 0; i < axisCount; i++ ){
            Carta::Lib::AxisInfo axisInfo = cf->axisInfo(i);
            if (axisInfo.knownType() == axisType){
                index = i;
                break;
            }
        }
    }
    return index;
}

Carta::Lib::NdArray::RawViewInterface* getRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                                                  int frameStart, int frameEnd, int stokeSliceIndex) {
    Carta::Lib::NdArray::RawViewInterface* rawData = nullptr;
    int m_axisIndexX = getAxisIndex(m_image, AxisInfo::KnownType::DIRECTION_LON);
    int m_axisIndexY = getAxisIndex(m_image, AxisInfo::KnownType::DIRECTION_LAT);
    int stokeIndex = getAxisIndex(m_image, AxisInfo::KnownType::STOKES);
    int spectralIndex = getAxisIndex(m_image, AxisInfo::KnownType::SPECTRAL);
    qDebug() << "++++++++ X Index:" << m_axisIndexX;
    qDebug() << "++++++++ Y Index:" << m_axisIndexY;
    qDebug() << "++++++++ Stoke Index:" << stokeIndex;
    qDebug() << "++++++++ Spectral Index:" << spectralIndex;

    if (m_axisIndexX != 0 || m_axisIndexY != 1) {
        qFatal("Can not find x or y axises");
    }

    if (m_image) {
        int imageDim = m_image->dims().size();
        qDebug() << "++++++++ the dimension of image:" << imageDim;
        SliceND frameSlice = SliceND().next();
        for (int i = 0; i < imageDim; i++) {
            if (i != m_axisIndexX && i != m_axisIndexY) {
                // get the number of slice (e.q. channel) in this dimension
                int sliceSize = m_image->dims()[i];
                // make slice cuts for the raw data
                SliceND& slice = frameSlice.next();
                // If it is the spectral axis
                if (i == spectralIndex) {
                   if (0 <= frameStart && frameStart < sliceSize &&
                       0 <= frameEnd && frameEnd < sliceSize){
                       slice.start(frameStart);
                       slice.end(frameEnd + 1);
                   } else {
                       slice.start(0);
                       slice.end(sliceSize);
                   }
                // If it is the stoke axis
                } else if (i == stokeIndex) {
                    if (stokeSliceIndex >= 0 && stokeSliceIndex <= 3) {
                        qDebug() << "++++++++ we only consider the stoke" << stokeSliceIndex <<
                                    "(-1: no stoke, 0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V) for percentile calculation" ;
                        slice.start(stokeSliceIndex);
                        slice.end(stokeSliceIndex + 1);
                    } else {
                        slice.start(0);
                        slice.end(sliceSize);
                    }
                // For the other axises
                } else {
                    slice.start(0);
                    slice.end(sliceSize);
                }
                slice.step(1);
            }
        }
        rawData = m_image->getDataSlice(frameSlice);
    }
    return rawData;
}

std::vector<double> getHertzValues(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                                   std::vector<int> dims, int spectralIndex) {
    std::vector<double> hertzValues;

    if (spectralIndex >= 0) { // multiple frames
        std::vector<double> Xvalues;

        for (int i = 0; i < dims[spectralIndex]; i++) {
            Xvalues.push_back((double)i);
        }

        // convert frame indices to Hz
        auto result = Globals::instance()-> pluginManager()-> prepare <Carta::Lib::Hooks::ConversionSpectralHook>(m_image, "", "Hz", Xvalues );
        auto lam = [&hertzValues] ( const Carta::Lib::Hooks::ConversionSpectralHook::ResultType &data ) {
            hertzValues = data;
        };

        result.forEach( lam );
    } else {
        qWarning() << "Could not calculate Hertz values. This image has no spectral axis.";
    }

    return hertzValues;
}

static void testPercentile(QString imageFname) {
    auto pm = Globals::instance()->pluginManager();

    // tell all plugins that the core has initialized
    pm-> prepare <Carta::Lib::Hooks::Initialize> ().executeAll();

    auto imgRes = pm-> prepare <Carta::Lib::Hooks::LoadAstroImage> (imageFname).first();
    if (imgRes.isNull()) {
        throw "Could not find any plugin to load astroImage";
    }

    //Carta::Lib::Image::ImageInterface::SharedPtr astroImage = imgRes.val();
    std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage = imgRes.val();
    if (! astroImage) {
        throw "Image read was a nullptr";
    }

    // set stoke index for getting the raw data (0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V)
    int stokeIndex = 0;

    // get raw data as the double type
    Carta::Lib::NdArray::RawViewInterface* rawData = getRawData(astroImage, -1, -1, stokeIndex);
    std::shared_ptr<Carta::Lib::NdArray::RawViewInterface> view(rawData);
    Carta::Lib::NdArray::Double doubleView(view.get(), false);

    //get the index of spectral axis
    int spectralIndex = getAxisIndex(astroImage, AxisInfo::KnownType::SPECTRAL);

    // TODO: set a unit converter for tests
    //Carta::Lib::IntensityUnitConverter::SharedPtr converter(ConverterIntensity::converters(from_units, to_units, max_value, max_units, BEAM_AREA));
    Carta::Lib::IntensityUnitConverter::SharedPtr converter=nullptr;

    // get Hertz values
    std::vector<double> hertzValues={};
    if (converter && converter->frameDependent) {
        hertzValues = getHertzValues(astroImage, doubleView.dims(), spectralIndex);
    }

    // calculate Min and Max percentiles: new way
    std::map<double, double> clips_MinMax1 = Carta::Core::Algorithms::minMax2pixels(doubleView, {0, 1}, spectralIndex, converter, hertzValues);
    double intensity_min_new = clips_MinMax1[0];
    double intensity_max_new = clips_MinMax1[1];

    // calculate Min and Max percentiles: original way
    std::map<double, double> clips_MinMax2 = Carta::Core::Algorithms::percentile2pixels(doubleView, {0, 1}, spectralIndex, converter, hertzValues);
    double intensity_min_original = clips_MinMax2[0];
    double intensity_max_original = clips_MinMax2[1];

    // calculate the intensity range
    double intensity_range = fabs(intensity_max_new-intensity_min_new);

    // calculate precise percentiles to intensities
    std::map<double, double>
            clips_map1 = Carta::Core::Algorithms::percentile2pixels(doubleView, percentile, spectralIndex, converter, hertzValues);

    // check the difference of Min/Max intensity with new and original algorithms
    qCritical() << "\n############################### START CHECKING #################################";
    qCritical() << "For the image file:" << imageFname;
    qCritical() << "--------------------------------------------------------------------------------";
    double delta_min = fabs(intensity_min_new-intensity_min_original);
    double delta_max = fabs(intensity_max_new-intensity_max_original);
    if (delta_min < 1.0e-9 && delta_max < 1.0e-9) {
        qCritical() << "[PASS] for Min/Max intensity =" << intensity_min_new << "/" << intensity_max_new;
    } else {
        qCritical() << "[FAIL !!] for Min/Max intensity =" << intensity_min_new << "/" << intensity_max_new << "(new way)"
                    << "!=" << intensity_min_original << "/" << intensity_max_original << "(original way)";
    }

    // calculate approximate percentiles to intensities
    int bin_number_size = bin_number.size();
    for (int j = 0; j < bin_number_size; j++) {
        std::map<double, double>
                clips_map2 = Carta::Core::Algorithms::percentile2pixels_approximation(doubleView,
                        {clips_MinMax1[0], clips_MinMax1[1]}, bin_number[j], percentile, spectralIndex, converter, hertzValues);

        // expected intensity error
        double expected_error = 100/bin_number[j]; // represented as %

        // check the difference of percentile to intensity with new and original algorithms
        int percentile_number = percentile.size();
        qCritical() << "--------------------------------------------------------------------------------";
        qCritical() << "For bin numbers:" << bin_number[j];
        for (int i = 0; i < percentile_number; i++) {
            double intensity_precise = clips_map1[percentile[i]];
            double intensity_approximation = clips_map2[percentile[i]];
            double error = 100*fabs(intensity_approximation - intensity_precise)/intensity_range; // represented as %
            if (error <= expected_error) {
                qCritical() << "[PASS] for percentile" << 100*percentile[i] << "(%), intensity error =" << error << "<=" << expected_error << "(%)";
            } else {
                qCritical() << "[FAIL !!] for percentile" << 100*percentile[i] << "(%), intensity error =" << error << ">" << expected_error << "(%)";
            }
        }
    }
    qCritical() << "#################################### END #######################################\n";

}

static int coreMainCPP(QString platformString, int argc, char **argv) {
    MyQApp qapp(argc, argv);

    QString appName = "carta-" + platformString;
    appName += "-verbose";

    if (CARTA_RUNTIME_CHECKS) {
        appName += "-runtimeChecks";
    }

    MyQApp::setApplicationName(appName);
    qDebug() << "Starting" << qapp.applicationName() << qapp.applicationVersion();

    // alias globals
    auto & globals = * Globals::instance();

    // parse command line arguments & environment variables
    auto cmdLineInfo = CmdLine::parse(MyQApp::arguments());
    globals.setCmdLineInfo(& cmdLineInfo);

    int file_num = cmdLineInfo.fileList().size();
    if (file_num < 1) {
        qFatal("Usage: ./testPercentile [the path and name of the image file] ...");
    }

    // load the config file
    QString configFilePath = cmdLineInfo.configFilePath();
    MainConfig::ParsedInfo mainConfig = MainConfig::parse(configFilePath);
    globals.setMainConfig(& mainConfig);
    qDebug() << "plugin directories:\n - " + mainConfig.pluginDirectories().join( "\n - " );

    // initialize plugin manager
    globals.setPluginManager(std::make_shared<PluginManager>());
    auto pm = globals.pluginManager();

    // tell plugin manager where to find plugins
    pm-> setPluginSearchPaths(globals.mainConfig()->pluginDirectories());

    // find and load plugins
    pm-> loadPlugins();

    qDebug() << "Loading plugins...";
    auto infoList = pm-> getInfoList();

    qDebug() << "List of loaded plugins: [" << infoList.size() << "]";
    for (const auto & entry : infoList) {
        qDebug() << "  path:" << entry.json.name;
    }

    // get percentile from the image raw data
    for (int i = 0; i < file_num; i++) {
        testPercentile(cmdLineInfo.fileList()[i]);
    }

    // if we get here, it means we are done
    qDebug() << "Done";
    int res = 0;

    return res;
} // coreMainCPP

} // namespace tPercentile

int main(int argc, char **argv) {
    try {
        return tPercentile::coreMainCPP( "tPercentile", argc, argv );
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
