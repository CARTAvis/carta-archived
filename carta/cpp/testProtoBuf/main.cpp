/*
 * This is the test for percentile algorithms and protocol buffer
 *
 * Usage: $./testProtoBuf [the path and name of the image file]
 *
 * for example: $./testProtoBuf.app/Contents/MacOS/testProtoBuf ../../../carta/scriptedClient/tests/data/aH.fits
 *
 */
#include "core/MyQApp.h"
#include "core/CmdLine.h"
#include "core/MainConfig.h"
#include "core/Globals.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/PercentileToPixelHook.h"

#include "core/State/ObjectManager.h"
#include "core/Data/DataLoader.h"
#include "core/Data/ViewManager.h"

#include <QDebug>
#include <cmath>
#include <QJsonObject>

#include <ctime>
#include <fstream>
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <string>

#include "stream.pb.h"
#include "request.pb.h"

#include "ProtoBufHelper.h"
#include "CartaHelper.h"

using namespace std;

typedef ::google::protobuf::uint32 uint32;

namespace testProtoBuff {

typedef Carta::Lib::AxisInfo AxisInfo;

static void test_raster_image(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage) {

    //*****************************************************************
    // set parameters for protocol buffer
    //*****************************************************************

    // The file ID that the raster image corresponds to
    uint32 file_id = 0;

    // The layer ID that the raster image corresponds to
    uint32 layer_id = 0;

    // Starting x-coordinate of the image region in image space
    uint32 x = 0;

    // Starting y-coordinate of the image region in image space
    uint32 y = 0;

    // Width (in the x-direction) of the image region in image space
    int x_size = astroImage->dims()[0]; // the size of X-axis
    uint32 width = x_size;

    // Height (in the y-direction) of the image region in image space
    int y_size = astroImage->dims()[1]; // the size of Y-axis
    uint32 height = y_size;

    // The image stokes coordinate
    uint32 stokes = 0;

    // The image channel (z-coordinate)
    uint32 channel = 0;

    // The mip level used. The mip level defines how many image pixels correspond to the downsampled image
    uint32 mip = 1;

    // The compression algorithm used.
    Stream::RasterImageData::CompressionType type = Stream::RasterImageData::NONE;

    // Compression quality switch
    float compression_quality = 0;

    // The number of subsets that the data is broken into (for multithreaded compression/decompression)
    uint32 num_subsets = 0;

    //*****************************************************************
    // set parameters for extracting the raw data
    //*****************************************************************

    // set the spectral frame range [frameStart, frameEnd]
    int frameStart = channel;
    int frameEnd = frameStart + 1;
    // set stoke index for getting the raw data (0: stoke I, 1: stoke Q, 2: stoke U, 3: stoke V)
    int stokeIndex = stokes;
    // set the range of pixel coordinates to extract the raw data
    int ilb = x;          // start of column index
    int iub = width - 1;  // end of column index
    int jlb = y;          // start of row index
    int jub = height - 1; // end of row index

    // The mip level defines how many image pixels correspond to the downsampled image
    int downSample = mip;

    // extract the raw data
    std::vector<float> rawdata = extractRawData(astroImage, frameStart, frameEnd, stokeIndex, ilb, iub, jlb, jub, downSample);

    //*****************************************************************
    // start to load the protocol buffer !!
    //*****************************************************************

    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Stream::DataBook DataBook;
    //char const *test = "DataBook.dat";

    // Add an databook.
    PromptForRasterImageData(DataBook.mutable_raster_image_data(), file_id, layer_id, x, y,
                             width, height, stokes, channel, mip, type, compression_quality, num_subsets, rawdata);

    // Write the new databook book back to disk.
    //fstream output(test, ios::out | ios::trunc | ios::binary);
    //if (!DataBook.SerializeToOstream(&output)) {
    //  cerr << "Failed to write databook book." << endl;
    //  return -1;
    //}

    // list the DataBook
    ListRasterImageData(DataBook);

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
}

static void test_file_list() {
    std::vector<Carta::Data::DataLoader::FileInfo> m_fileLists; // list of available image files with file name, type and size information
    std::vector<QString> m_dirLists; // list of available subdirectories
    QString requestDir = "/Users/mark/CARTA/Images/testStoke"; // required directory name
    int resursionLevel = 2; // setting this parameter to zero will result in only files in the requested directory being shown

    extractFileList(requestDir, resursionLevel, m_fileLists, m_dirLists);

    for (auto m_fileList : m_fileLists) {
        qDebug() << "m_name:" << m_fileList.name << ", m_type:" << m_fileList.type << ", m_size:" << m_fileList.size;
    }
    for (auto m_dirList : m_dirLists) {
        qDebug() << "m_dir:" << m_dirList;
    }
}

static int coreMainCPP(QString platformString, int argc, char* argv[]) {
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
        qFatal("Usage: ./testestProtoBuff [the path and name of the image file] ...");
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
    pm -> setPluginSearchPaths(globals.mainConfig()->pluginDirectories());

    // find and load plugins
    pm -> loadPlugins();

    qDebug() << "Loading plugins...";
    auto infoList = pm -> getInfoList();

    qDebug() << "List of loaded plugins: [" << infoList.size() << "]";
    for (const auto & entry : infoList) {
        qDebug() << "  path:" << entry.json.name;
    }

    // tell all plugins that the core has initialized
    pm -> prepare<Carta::Lib::Hooks::Initialize>().executeAll();

    //*****************************************************************
    // load the image file
    //*****************************************************************

    auto imgRes = pm -> prepare<Carta::Lib::Hooks::LoadAstroImage>(cmdLineInfo.fileList()[0]).first();

    if (imgRes.isNull()) {
        throw "Could not find any plugin to load astroImage";
    }

    std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage = imgRes.val();

    if (!astroImage) {
        throw "Image read was a nullptr";
    }

    //*****************************************************************
    // test protocol buffer for the raster image data
    //*****************************************************************

    test_raster_image(astroImage);

    //*****************************************************************
    // test protocol buffer for the file list
    //*****************************************************************

    test_file_list();

    return 0;
} // coreMainCPP

} // namespace testProtoBuff

int main(int argc, char* argv[]) {
    try {
        return testProtoBuff::coreMainCPP( "testProtoBuff", argc, argv );
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
