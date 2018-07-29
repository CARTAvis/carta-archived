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
#include "CartaLib/IImage.h"

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
    if (length < 36){
        qFatal("Illegal message.");
        return;
    }

    int nullIndex = 0;
    for (int i = 0; i < 32; i++) {
        if (!message[i]) {
            nullIndex = i;
            break;
        }
    }

    QString eventName = QString::fromStdString(std::string(message, nullIndex));
    qDebug() << "Event received: " << eventName;

    QString respName;
    std::vector<char> result;
    PBMSharedPtr msg;

    if (eventName == "REGISTER_VIEWER") {
        // The message should be handled in sessionDispatcher
        qFatal("Illegal request in NewServerConnector. Please handle it in SessionDispatcher.");
        return;
    }
    else if (eventName == "FILE_LIST_REQUEST"){
        respName = "FILE_LIST_RESPONSE";

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        Carta::Data::DataLoader *dataLoader = objMan->createObject<Carta::Data::DataLoader>();

        CARTA::FileListRequest fileListRequest;
        fileListRequest.ParseFromArray(message + 36, length - 36);
        msg = dataLoader->getFileList(fileListRequest);
    }
    else if (eventName == "FILE_INFO_REQUEST") {
        respName = "FILE_INFO_RESPONSE";

        // we cannot handle the request so far, return a fake response.
        std::shared_ptr<CARTA::FileInfoResponse> fileInfoResponse(new CARTA::FileInfoResponse());
        fileInfoResponse->set_success(true);
        msg = fileInfoResponse;
    }
    else if (eventName == "OPEN_FILE") {
        respName = "OPEN_FILE_ACK";

        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        QString controllerID = this->viewer.m_viewManager->registerView("pluginId:ImageViewer,index:0").split("/").last();
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>( objMan->getObject(controllerID) );
        bool success;

        CARTA::OpenFile openFile;
        openFile.ParseFromArray(message + 36, length - 36);
        controller->addData(QString::fromStdString(openFile.directory()) + "/" + QString::fromStdString(openFile.file()), &success);

        std::shared_ptr<Carta::Lib::Image::ImageInterface> image = controller->getImage();

        CARTA::FileInfo* fileInfo = new CARTA::FileInfo();
        fileInfo->set_name(openFile.file());
        if (image->getType() == "FITSImage") {
            fileInfo->set_type(CARTA::FileType::FITS);
        }
        else {
            fileInfo->set_type(CARTA::FileType::CASA);
        }
        fileInfo->add_hdu_list(openFile.hdu());

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
    }
    else if (eventName == "SET_IMAGE_VIEW") {
        respName = "RASTER_IMAGE_DATA";
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        QString controllerID = this->viewer.m_viewManager->registerView("pluginId:ImageViewer,index:0").split("/").last();
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>( objMan->getObject(controllerID) );
        bool success;

        CARTA::SetImageView viewSetting;
        viewSetting.ParseFromArray(message + 36, length - 36);

        std::vector<int> frames = controller->getImageSlice();
        Carta::Lib::NdArray::RawViewInterface* view = controller->getRawData();
        // Carta::Lib::NdArray::Float fview( view, true );

        const std::vector<int> dims = view->dims();
        // std::vector<float> data;
        std::vector<char> data;
        size_t i = 0;
        // data.resize(dims[0]*dims[1]);
        view->forEach([&data, &i](const char * val){
            data.push_back(*val);
        });
        // fview.forEach([&data, &i](const float & val){
        //     data[i] = val;
        // });

        // Carta::Lib::NdArray::Float floatView(view, false);
        // std::vector<float> allValues;
        // // get all pixel elements
        // floatView.forEach([& allValues] (const float & val) {
        //     if (std::isfinite(val)) {
        //         allValues.push_back(val);
        //     } else {
        //         allValues.push_back(0.0);
        //     }
        // });

///////////////////////////copy from Mark////////////////////////////////////////////
    int mip = viewSetting.mip();
    std::vector<float> allValues;
    int ilb = 0;
    int iub = dims[0] - 1;
    int jlb = 0;
    int jub = dims[1] - 1;
    int nRows = (jub - jlb + 1) / mip;
    int nCols = (iub - ilb + 1) / mip;

    // if (nCols > view -> dims()[0] || nRows > view -> dims()[1] || nCols < 0 || nRows < 0 ||
    //     mip < 1 || iub < 0 || ilb < 0 || jub < 0 || jlb < 0) {
    //     qCritical() << "The input values of ilb, iub, jlb, jub or mip may be not correct!";
    //     return allValues;
    // }

    int prepareCols = iub - ilb + 1;
    int prepareRows = mip;
    int area = prepareCols * prepareRows;
    std::vector<float> rawData(nCols), prepareArea(area);
    int nextRowToReadIn = jlb;

    Carta::Lib::NdArray::Float fview( view, true );

    auto updateRows = [&]() -> void {

        int update = prepareRows;

        int t = 0;
        fview.forEach( [&] ( const float & val ) {
            // To improve the performance, the prepareArea also update only one row
            // by computing the module
            prepareArea[(t++) % area] = val;
        });

        // Calculate the mean of each block (mip X mip)
        for (int i = ilb; i < nCols; i++) {
            rawData[i] = 0;
            int elems = mip * mip;
            float denominator = 1.0 * elems;
            for (int e = 0; e < elems; e++) {
                int row = e / mip;
                int col = e % mip;
                int index = (row * prepareCols + col + i * mip) % area;
                if (std::isfinite(prepareArea[index])) {
                    rawData[i] += prepareArea[index];
                } else {
                    denominator -= 1;
                }
            }
            rawData[i] = (denominator < 1 ? 0.0 : rawData[i] / denominator);
            allValues.push_back(rawData[i]);
        }
        nextRowToReadIn += update;
    };

    for (int j = jlb; j < nRows; j++) {
        updateRows();
    }

/////////////////////////////////////////////////////////////////////


        CARTA::ImageBounds* imgBounds = new CARTA::ImageBounds();
        imgBounds->CopyFrom(viewSetting.image_bounds());

        std::shared_ptr<CARTA::RasterImageData> raster(new CARTA::RasterImageData());
        raster->set_file_id(viewSetting.file_id());
        raster->set_allocated_image_bounds(imgBounds);
        raster->set_channel(frames[2]);
        raster->set_stokes(frames[3]);
        raster->set_mip(viewSetting.mip());
        // raster->set_mip(1);
        // raster->set_compression_type(viewSetting.compression_type());
        raster->set_compression_type(CARTA::CompressionType::NONE);
        raster->set_compression_quality(viewSetting.compression_quality());
        raster->add_image_data(allValues.data(), allValues.size() * sizeof(float));
        // raster->add_image_data(data.data(), data.size());

        msg = raster;

    }
    else {
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
    }

    size_t eventNameLength = 32;
    size_t eventIdLength = 4;
    int messageLength = msg->ByteSize();
    size_t requiredSize = eventNameLength + eventIdLength + messageLength;
    if (result.size() < requiredSize) {
        result.resize(requiredSize);
    }
    memset(result.data(), 0, eventNameLength);
    memcpy(result.data(), respName.toStdString().c_str(), std::min<size_t>(respName.length(), eventNameLength));
    memcpy(result.data() + eventNameLength, message + eventNameLength, eventIdLength);
    if (msg) {
        msg->SerializeToArray(result.data() + eventNameLength + eventIdLength, messageLength);
        emit jsBinaryMessageResultSignal(result.data(), requiredSize);
        qDebug() << "Send event:" << respName << QTime::currentTime().toString();
    }
    // socket->send(binaryPayloadCache.data(), requiredSize, uWS::BINARY);

    // emit jsTextMessageResultSignal(result);
    //emit jsBinaryMessageResultSignal(result.data(), requiredSize);
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
