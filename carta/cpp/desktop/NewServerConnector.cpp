/**
 *
 **/

#include "NewServerConnector.h"
#include "CartaLib/LinearMap.h"
#include "core/MyQApp.h"
#include "core/SimpleRemoteVGView.h"
#include "core/State/ObjectManager.h"
#include "core/Data/DataLoader.h"
#include "core/Data/ViewManager.h"
#include "core/Data/Image/Controller.h"
#include "core/Data/Image/DataSource.h"
#include <iostream>
#include <QImage>
#include <QPainter>
#include <QXmlInputSource>
#include <cmath>
#include <QTime>
#include <QTimer>
#include <QCoreApplication>
#include <functional>

#include <QStringList>
#include <QBuffer>

#include <QThread>

#include "CartaLib/Proto/file_list.pb.h"
#include "CartaLib/Proto/file_info.pb.h"
#include "CartaLib/Proto/open_file.pb.h"
#include "CartaLib/Proto/set_image_view.pb.h"
#include "CartaLib/Proto/raster_image.pb.h"
#include "CartaLib/Proto/spectral_profile.pb.h"
#include "CartaLib/Proto/spatial_profile.pb.h"
#include "CartaLib/Proto/set_image_channels.pb.h"
#include "CartaLib/Proto/set_cursor.pb.h"
#include "CartaLib/Proto/region_stats.pb.h"
#include "CartaLib/Proto/region_requirements.pb.h"
#include "CartaLib/Proto/region_histogram.pb.h"
#include "CartaLib/Proto/region.pb.h"
#include "CartaLib/Proto/error.pb.h"
#include "CartaLib/Proto/contour_image.pb.h"
#include "CartaLib/Proto/contour.pb.h"
#include "CartaLib/Proto/close_file.pb.h"
#include "CartaLib/Proto/animation.pb.h"

#include "CartaLib/IImage.h"

// File_Info implementation test
// #include "FitsHeaderExtractor.h"
#include "CartaLib/Hooks/ImageStatisticsHook.h"
#include "Globals.h"

/// \brief internal class of NewServerConnector, containing extra information we like
///  to remember with each view
///
struct NewServerConnector::ViewInfo
{

    /// pointer to user supplied IView
    /// this is a NON-OWNING pointer
    IView * view;

    /// last received client size
    QSize clientSize;

    /// linear maps convert x,y from client to image coordinates
    Carta::Lib::LinearMap1D tx, ty;

    /// refresh timer for this object
    QTimer refreshTimer;

    /// refresh ID
    qint64 refreshId = -1;

    ViewInfo( IView * pview )
    {
        view = pview;
        clientSize = QSize(1,1);
        refreshTimer.setSingleShot( true);
        // just long enough that two successive calls will result in only one redraw :)
        refreshTimer.setInterval( 1000 / 120);
    }

};

NewServerConnector::NewServerConnector()
{
    // // queued connection to prevent callbacks from firing inside setState
    // connect( this, & NewServerConnector::stateChangedSignal,
    //          this, & NewServerConnector::stateChangedSlot,
    //          Qt::QueuedConnection );

    m_callbackNextId = 0;
}

NewServerConnector::~NewServerConnector()
{
}

void NewServerConnector::initialize(const InitializeCallback & cb)
{
    m_initializeCallback = cb;
}

// The function was initially implemented for flushstate()
// Deprecated since newArch, remove the func after removing Hack directory
void NewServerConnector::setState(const QString& path, const QString & newValue)
{
    // // find the path
    // auto it = m_state.find( path);

    // // if we cannot find it, insert it, together with the new value, and emit a change
    // if( it == m_state.end()) {
    //     m_state[path] = newValue;
    //     emit stateChangedSignal( path, newValue);
    //     return;
    // }

    // // if we did find it, but the value is different, set it to new value and emit signal
    // if( it-> second != newValue) {
    //     it-> second = newValue;
    //     emit stateChangedSignal( path, newValue);
    // }

    // // otherwise there was no change to state, so do dothing
}


QString NewServerConnector::getState(const QString & path  )
{
    return m_state[ path ];
}


/// Return the location where the state is saved.
QString NewServerConnector::getStateLocation( const QString& saveName ) const {
	// \todo Generalize this.
	return "/tmp/"+saveName+".json";
}

IConnector::CallbackID NewServerConnector::addCommandCallback(
        const QString & cmd,
        const IConnector::CommandCallback & cb)
{
    m_commandCallbackMap[cmd].push_back( cb);
    return m_callbackNextId++;
}

IConnector::CallbackID NewServerConnector::addMessageCallback(
        const QString & cmd,
        const IConnector::MessageCallback & cb)
{
    m_messageCallbackMap[cmd].push_back( cb);
    return m_callbackNextId++;
}

IConnector::CallbackID NewServerConnector::addStateCallback(
        IConnector::CSR path,
        const IConnector::StateChangedCallback & cb)
{
    // find the list of callbacks for this path
    auto iter = m_stateCallbackList.find( path);

    // if it does not exist, create it
    if( iter == m_stateCallbackList.end()) {
//        qDebug() << "Creating callback list for variable " << path;
        auto res = m_stateCallbackList.insert( std::make_pair(path, new StateCBList));
        iter = res.first;
    }

//    iter = m_stateCallbackList.find( path);
//    if( iter == m_stateCallbackList.end()) {
////        qDebug() << "What the hell";
//    }

    // add the calllback
    return iter-> second-> add( cb);

//    return m_stateCallbackList[ path].add( cb);
}

void NewServerConnector::registerView(IView * view)
{
    // let the view know it's registered, and give it access to the connector
    view->registration( this);

    // insert this view int our list of views
    ViewInfo * viewInfo = new ViewInfo( view);
//    viewInfo-> view = view;
//    viewInfo-> clientSize = QSize(1,1);
    m_views[ view-> name()] = viewInfo;

    // connect the view's refresh timer to a lambda, which will in turn call
    // refreshViewNow()
    // this is instead of using std::bind...
    // connect( & viewInfo->refreshTimer, & QTimer::timeout,
    //         [=] () {
    //                  refreshViewNow( view);
    // });
}

// unregister the view
void NewServerConnector::unregisterView( const QString& viewName ){
    ViewInfo* viewInfo = this->findViewInfo( viewName );
    if ( viewInfo != nullptr ){

        (& viewInfo->refreshTimer)->disconnect();
        m_views.erase( viewName );
    }
}

//    static QTime st;

// schedule a view refresh
qint64 NewServerConnector::refreshView(IView * view)
{
    // find the corresponding view info
    ViewInfo * viewInfo = findViewInfo( view-> name());
    if( ! viewInfo) {
        // this is an internal error...
        qCritical() << "refreshView cannot find this view: " << view-> name();
        return -1;
    }

    // start the timer for this view if it's not already started
//    if( ! viewInfo-> refreshTimer.isActive()) {
//        viewInfo-> refreshTimer.start();
//    }
//    else {
//        qDebug() << "########### saved refresh for " << view->name();
//    }

    // refreshViewNow(view);

    viewInfo-> refreshId ++;
    return viewInfo-> refreshId;
}

void NewServerConnector::removeStateCallback(const IConnector::CallbackID & /*id*/)
{
    qFatal( "not implemented");
}


Carta::Lib::IRemoteVGView * NewServerConnector::makeRemoteVGView(QString viewName)
{
    return new Carta::Core::SimpleRemoteVGView( this, viewName, this);
}

NewServerConnector::ViewInfo * NewServerConnector::findViewInfo( const QString & viewName)
{
    auto viewIter = m_views.find( viewName);
    if( viewIter == m_views.end()) {
        qWarning() << "NewServerConnector::findViewInfo: Unknown view " << viewName;
        return nullptr;
    }

    return viewIter-> second;
}

IConnector* NewServerConnector::getConnectorInMap(const QString & sessionID){
    return nullptr;
}

void NewServerConnector::setConnectorInMap(const QString & sessionID, IConnector *connector){
}

void NewServerConnector::startWebSocket(){
    // qFatal('NewServerConnector should not start a websocket!');
    CARTA_ASSERT_X( false, "NewServerConnector should not start a websocket!");
}

void NewServerConnector::startViewerSlot(const QString & sessionID) {

    QString name = QThread::currentThread()->objectName();
    // qDebug() << "current thread name:" << name;
    if (name != sessionID) {
        qDebug()<< "ignore startViewerSlot";
        return;
    }

    viewer.start();
}

void NewServerConnector::onTextMessage(QString message){
    QString controllerID = this->viewer.m_viewManager->registerView("pluginId:ImageViewer,index:0");
    QString cmd = controllerID + ":" + message;

    qDebug() << "Message received:" << message;
    auto & allCallbacks = m_messageCallbackMap[ message];

    // QString result;
    std::string data;
    PBMSharedPtr msg;

    for( auto & cb : allCallbacks ) {
        msg = cb( message, "", "1");
    }
    msg->SerializeToString(&data);
    const QString result = QString::fromStdString(data);

    if( allCallbacks.size() == 0) {
        qWarning() << "JS command has no server listener:" << message;
    }

    emit jsTextMessageResultSignal(result);
}

void NewServerConnector::onBinaryMessage(char* message, size_t length){
    if (length < EVENT_NAME_LENGTH + EVENT_ID_LENGTH){
        qFatal("Illegal message.");
        return;
    }

    size_t nullIndex = 0;
    for (size_t i = 0; i < EVENT_NAME_LENGTH; i++) {
        if (!message[i]) {
            nullIndex = i;
            break;
        }
    }

    QString eventName = QString::fromStdString(std::string(message, nullIndex));
    qDebug() << "Event received: " << eventName << QTime::currentTime().toString();

    QString respName;
    PBMSharedPtr msg;

    if (eventName == "REGISTER_VIEWER") {
        // The message should be handled in sessionDispatcher
        qFatal("Illegal request in NewServerConnector. Please handle it in SessionDispatcher.");
        return;

    } else if (eventName == "FILE_LIST_REQUEST") {
        respName = "FILE_LIST_RESPONSE";

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        Carta::Data::DataLoader *dataLoader = objMan->createObject<Carta::Data::DataLoader>();

        CARTA::FileListRequest fileListRequest;
        fileListRequest.ParseFromArray(message + EVENT_NAME_LENGTH + EVENT_ID_LENGTH, length - EVENT_NAME_LENGTH - EVENT_ID_LENGTH);
        msg = dataLoader->getFileList(fileListRequest);

        // send the serialized message to the frontend
        sendSerializedMessage(message, respName, msg);
        return;

    } else if (eventName == "FILE_INFO_REQUEST") {
        respName = "FILE_INFO_RESPONSE";

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        QString controllerID = this->viewer.m_viewManager->registerView("pluginId:ImageViewer,index:0").split("/").last();
        bool success;
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>( objMan->getObject(controllerID) );
        CARTA::FileInfoRequest openFile;
        openFile.ParseFromArray(message + EVENT_NAME_LENGTH + EVENT_ID_LENGTH, length - EVENT_NAME_LENGTH - EVENT_ID_LENGTH);
        controller->addData(QString::fromStdString(openFile.directory()) + "/" + QString::fromStdString(openFile.file()), &success);

        std::shared_ptr<Carta::Lib::Image::ImageInterface> image = controller->getImage();
        std::vector< std::shared_ptr<Carta::Lib::Image::ImageInterface> > images;
        images.push_back(image);

        CARTA::FileInfo* fileInfo = new CARTA::FileInfo();

        // FileInfo: name
        fileInfo->set_name(openFile.file());

        // FileInfo: type
        if (image->getType() == "FITSImage") {
            fileInfo->set_type(CARTA::FileType::FITS);
        } else {
            fileInfo->set_type(CARTA::FileType::CASA);
        }

        // FileInfo: size

        // FileInfoExtended

        const std::vector<int> dims = image->dims();
        CARTA::FileInfoExtended* fileInfoExt = new CARTA::FileInfoExtended();
        fileInfoExt->set_dimensions(dims.size());
        fileInfoExt->set_width(dims[0]);
        fileInfoExt->set_height(dims[1]);
        if (dims.size() >= 3) {
            fileInfoExt->set_depth(dims[2]);
        }
        if (dims.size() >= 4) {
            fileInfoExt->set_stokes(dims[3]);
        }

        // Prepare to use the ImageStats plugin.
        std::vector<std::shared_ptr<Carta::Lib::Regions::RegionBase> > regions;
        std::vector<int> frameIndices = controller->getImageSlice();

        int sourceCount = images.size();
        if ( sourceCount > 0 ){
            auto result = Globals::instance()-> pluginManager()
                         -> prepare <Carta::Lib::Hooks::ImageStatisticsHook>(images, regions, frameIndices);
            auto lam = [=] ( const Carta::Lib::Hooks::ImageStatisticsHook::ResultType &data ) {
              //An array for each image
              int dataCount = data.size();
              // m_stateData.resizeArray( STATS, dataCount );
              for ( int i = 0; i < dataCount; i++ ){
                  //Each element of the image array contains an array of statistics.
                  // QString arrayLookup = Carta::State::UtilState::getLookup( "stats", i );
                  int statCount = data[i].size();
                  // m_stateData.setArray( arrayLookup, statCount );

                  //Go through each set of statistics for the image.
                  for ( int k = 0; k < statCount; k++ ){
                      // QString objLookup = Carta::State::UtilState::getLookup( arrayLookup, k );

                      // QList<QString> existingKeys = m_stateData.getMemberNames( objLookup );
                      int keyCount = data[i][k].size();
                      for ( int j = 0; j < keyCount; j++ ){
                          QString label = data[i][k][j].getLabel();
                          QString value = data[i][k][j].getValue();
                          // QString lookup = Carta::State::UtilState::getLookup( objLookup, label );
                          CARTA::HeaderEntry* headerEntry = fileInfoExt->add_header_entries();
                          headerEntry->set_name(label.toLocal8Bit().constData());
                          headerEntry->set_value(value.toLocal8Bit().constData());
                          // qDebug() << "label:" << label << "lookup" << lookup << "Value: " << data[i][k][j].getValue();
                          // m_stateData.insertValue<QString>( lookup, data[i][k][j].getValue() );
                      }
                  }
              }
              // m_stateData.flushState();
            };
            try {
                result.forEach( lam );
            }
            catch( char*& error ){
                QString errorStr( error );
                qDebug() << "There is an error message: " << error;
                // Carta::Data::ErrorManager* hr = Carta::State::Util::findSingletonObject<Carta::Data::ErrorManager>();
                // hr->registerError( errorStr );
            }
        }

        // FileInfoResponse
        std::shared_ptr<CARTA::FileInfoResponse> fileInfoResponse(new CARTA::FileInfoResponse());
        fileInfoResponse->set_success(true);
        fileInfoResponse->set_allocated_file_info(fileInfo);
        fileInfoResponse->set_allocated_file_info_extended(fileInfoExt);
        msg = fileInfoResponse;

        // send the serialized message to the frontend
        sendSerializedMessage(message, respName, msg);
        return;

    } else if (eventName == "OPEN_FILE") {
        respName = "OPEN_FILE_ACK";

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        QString controllerID = this->viewer.m_viewManager->registerView("pluginId:ImageViewer,index:0").split("/").last();
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>( objMan->getObject(controllerID) );
        bool success;

        CARTA::OpenFile openFile;
        // openFile.ParseFromArray(message + EVENT_NAME_LENGTH + EVENT_ID_LENGTH, length);
        openFile.ParseFromArray(message + EVENT_NAME_LENGTH + EVENT_ID_LENGTH, length - EVENT_NAME_LENGTH - EVENT_ID_LENGTH);

        QString fileDir = QString::fromStdString(openFile.directory());
        QString fileName = QString::fromStdString(openFile.file());
        if (!QDir(fileDir).exists()) {
            qWarning() << "File directory doesn't exist! (" << fileDir << ")";
            return;
        }

        controller->addData(fileDir + "/" + fileName, &success);

        if (success) {
            m_changeImage = true;
            qDebug() << "Image file (or channel) changed, re-calculate the pixels to histogram data!";
        }

        std::shared_ptr<Carta::Lib::Image::ImageInterface> image = controller->getImage();

        CARTA::FileInfo* fileInfo = new CARTA::FileInfo();
        fileInfo->set_name(openFile.file());

        if (image->getType() == "FITSImage") {
            fileInfo->set_type(CARTA::FileType::FITS);
        } else {
            fileInfo->set_type(CARTA::FileType::CASA);
        }
        // fileInfo->add_hdu_list(openFile.hdu());

        const std::vector<int> dims = image->dims();
        CARTA::FileInfoExtended* fileInfoExt = new CARTA::FileInfoExtended();
        fileInfoExt->set_dimensions(dims.size());
        fileInfoExt->set_width(dims[0]);
        fileInfoExt->set_height(dims[1]);
        if (dims.size() >= 3) {
            fileInfoExt->set_depth(dims[2]);
        }
        if (dims.size() >= 4) {
            fileInfoExt->set_stokes(dims[3]);
        }
        // CARTA::HeaderEntry* headEntry = fileInfoExt->add_header_entries();

        // we cannot handle the request so far, return a fake response.
        std::shared_ptr<CARTA::OpenFileAck> ack(new CARTA::OpenFileAck());
        ack->set_success(true);
        ack->set_file_id(openFile.file_id());
        ack->set_allocated_file_info(fileInfo);
        ack->set_allocated_file_info_extended(fileInfoExt);
        msg = ack;

        // send the serialized message to the frontend
        sendSerializedMessage(message, respName, msg);
        return;

    } else if (eventName == "SET_IMAGE_VIEW") {
        CARTA::SetImageView viewSetting;
        viewSetting.ParseFromArray(message + EVENT_NAME_LENGTH + EVENT_ID_LENGTH, length - EVENT_NAME_LENGTH - EVENT_ID_LENGTH);

        // get the controller
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        QString controllerID = this->viewer.m_viewManager->registerView("pluginId:ImageViewer,index:0").split("/").last();
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>( objMan->getObject(controllerID) );

        //qDebug() << "File ID requested by frontend:" << viewSetting.file_id();
        int frameLow = 0;
        int frameHigh = frameLow;
        int stokeFrame = 0;

        /////////////////////////////////////////////////////////////////////
        if (m_changeImage) {
            qDebug() << "Calculating the regional histogram data....................................";

            respName = "REGION_HISTOGRAM_DATA";

            // calculate pixels to histogram data
            int numberOfBins = 10000;
            Carta::Lib::IntensityUnitConverter::SharedPtr converter = nullptr; // do not include unit converter for pixel values
            RegionHistogramData regionHisotgramData = controller->getPixels2Histogram(frameLow, frameHigh, numberOfBins, stokeFrame, converter);
            std::vector<uint32_t> pixels2histogram = regionHisotgramData.bins;
            // the minimum value of pixels is the first bin center
            m_minIntensity = regionHisotgramData.first_bin_center;

            // add RegionHistogramData message
            std::shared_ptr<CARTA::RegionHistogramData> region_histogram_data(new CARTA::RegionHistogramData());
            region_histogram_data->set_file_id(viewSetting.file_id());

            // If the histograms correspond to the entire current 2D image, the region ID has a value of -1.
            region_histogram_data->set_region_id(-1);

            region_histogram_data->set_stokes(stokeFrame);

            CARTA::Histogram* histogram = region_histogram_data->add_histograms();
            histogram->set_channel(frameLow);
            histogram->set_num_bins(regionHisotgramData.num_bins);
            histogram->set_bin_width(regionHisotgramData.bin_width);
            histogram->set_first_bin_center(m_minIntensity);

            for (auto intensity : pixels2histogram) {
                histogram->add_bins(intensity);
            }

            msg = region_histogram_data;

            qDebug() << ".......................................................................Done";

            // send the serialized message to the frontend
            sendSerializedMessage(message, respName, msg);

            // set m_changeImage = false, in order to avoid the re-calculation of pixels to histogram
            //m_changeImage = false;
        }
        /////////////////////////////////////////////////////////////////////

        int mip = viewSetting.mip();
        int x_min = viewSetting.image_bounds().x_min();
        int x_max = viewSetting.image_bounds().x_max();
        int y_min = viewSetting.image_bounds().y_min();
        int y_max = viewSetting.image_bounds().y_max();

        if (m_changeImage) {
            // set m_changeImage = false, in order to avoid the re-calculation of pixels to histogram
            m_changeImage = false;
        } else if (mip == m_mip && x_min == m_xMin && x_max == m_xMax && y_min == m_yMin && y_max == m_yMax) {
            // if the required region of image viewer from frontend is the same with the previous requirement, ignore it.
            qDebug() << "Image boundary settings are repeated.";
            return;
        } else {
            qDebug() << "Cache the new image boundary settings.";
            m_mip = mip;
            m_xMin = x_min;
            m_xMax = x_max;
            m_yMin = y_min;
            m_yMax = y_max;
        }

        /////////////////////////////////////////////////////////////////////
        qDebug() << "Down sampling the raster image data........................................";

        respName = "RASTER_IMAGE_DATA";

        qDebug() << "Dawn sampling factor mip:" << mip;

        int W = (x_max - x_min) / mip;
        int H = (y_max - y_min) / mip;
        qDebug() << "get the x-pixel-coordinate range: [x_min, x_max]= [" << x_min << "," << x_max << "]"
                 << "--> W=" << W;
        qDebug() << "get the y-pixel-coordinate range: [y_min, y_max]= [" << y_min << "," << y_max << "]"
                 << "--> H=" << H;

        // get the down sampling raster image raw data
        std::vector<float> imageData = controller->getRasterImageData(x_min, x_max, y_min, y_max, mip,
            m_minIntensity, frameLow, frameHigh, stokeFrame);
        qDebug() << "number of the raw data sent L=" << imageData.size() << ", WxH=" << W * H
                 << ", Difference:" << (W * H - imageData.size());

        // add the RasterImageData message
        CARTA::ImageBounds* imgBounds = new CARTA::ImageBounds();
        imgBounds->CopyFrom(viewSetting.image_bounds());

        std::shared_ptr<CARTA::RasterImageData> raster(new CARTA::RasterImageData());
        raster->set_file_id(viewSetting.file_id());
        raster->set_allocated_image_bounds(imgBounds);
        raster->set_channel(frameLow);
        raster->set_stokes(stokeFrame);
        raster->set_mip(viewSetting.mip());
        // raster->set_compression_type(viewSetting.compression_type());
        raster->set_compression_type(CARTA::CompressionType::NONE);
        raster->set_compression_quality(viewSetting.compression_quality());
        raster->add_image_data(imageData.data(), imageData.size() * sizeof(float));

        msg = raster;

        qDebug() << ".......................................................................Done";

        // send the serialized message to the frontend
        sendSerializedMessage(message, respName, msg);
        return;
        /////////////////////////////////////////////////////////////////////

    } else {
        // Insert non-global object id
        // QString controllerID = this->viewer.m_viewManager->registerView("pluginId:ImageViewer,index:0");
        // QString cmd = controllerID + ":" + eventName;
        auto & allCallbacks = m_messageCallbackMap[eventName];

        if (allCallbacks.size() == 0) {
            qCritical() << "There is no event handler:" << eventName;
            return;
        }

        for( auto & cb : allCallbacks ) {
            msg = cb( eventName, "", "1");
        }

        return;
    }

    // socket->send(binaryPayloadCache.data(), requiredSize, uWS::BINARY);
    // emit jsTextMessageResultSignal(result);
    return;
}

void NewServerConnector::sendSerializedMessage(char* message, QString respName, PBMSharedPtr msg) {
    bool success = false;
    size_t requiredSize = 0;
    std::vector<char> result = serializeToArray(message, respName, msg, success, requiredSize);
    if (success) {
        emit jsBinaryMessageResultSignal(result.data(), requiredSize);
        qDebug() << "Send event:" << respName << QTime::currentTime().toString();
    }

}

// void NewServerConnector::stateChangedSlot(const QString & key, const QString & value)
// {
//     // find the list of callbacks for this path
//     auto iter = m_stateCallbackList.find( key);

//     // if it does not exist, do nothing
//     if( iter == m_stateCallbackList.end()) {
//         return;
//     }

//     // call all registered callbacks for this key
//     iter-> second-> callEveryone( key, value);
// }
