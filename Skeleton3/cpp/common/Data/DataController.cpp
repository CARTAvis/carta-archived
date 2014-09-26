#include "DataController.h"
#include "Globals.h"
#include "IConnector.h"

#include "State/StateLibrary.h"
#include "Data/DataSource.h"
#include "Data/DataSelection.h"
#include "TestView.h"

#include <QDebug>

DataController::DataController() :
        m_selectChannel(nullptr), m_selectImage(nullptr), m_view(nullptr) {

     //Listen for updates to the clip and reload the frame.
     auto & globals = *Globals::instance();
     IConnector* connector = globals.connector();
     QString clipValuePath = StateLibrary::instance()->getPath( StateKey::CLIP_VALUE, "");
     connector->addStateCallback(clipValuePath,
                [=] ( const QString & /*path*/, const QString & /*val*/) {
             if ( m_view ){
                    _loadView( true);
             }
     });
}

void DataController::addData(const QString& fileName) {
    //Find the location of the data, if it already exists.
    int targetIndex = -1;
    for (int i = 0; i < m_datas.size(); i++) {
        if (m_datas[i]->contains(fileName)) {
            targetIndex = i;
            break;
        }
    }

    //Add the data if it is not already there.
    if (targetIndex == -1) {
        std::shared_ptr<DataSource> targetSource(new DataSource(fileName));

        targetIndex = m_datas.size();
        m_datas.push_back(targetSource);

        //Update the data selectors upper bound and index based on the data.
        m_selectImage->setUpperBound(m_datas.size());

        saveState();
    }
    int frameCount = m_datas[targetIndex]->getFrameCount();
    m_selectChannel->setUpperBound( frameCount );
    m_selectImage->setIndex(targetIndex);

    //Refresh the view of the data.
    _loadView( false );

    //Notify others there has been a change to the data.
    emit dataChanged();
}


void DataController::setId(const QString& winId) {
    if ( m_winId != winId ){
        m_winId = winId;

        //Reset the view
        m_view.reset(new TestView(m_winId, QColor("pink"), QImage()));
        auto & globals = *Globals::instance();
        IConnector * connector = globals.connector();
        connector->registerView(m_view.get());

        //Reset the selections based on the new id.
        QString endPath = StateLibrary::instance()->getPath( StateKey::FRAME_IMAGE_END, m_winId );
        m_selectChannel.reset(
            new DataSelection(m_winId, StateKey::FRAME_CHANNEL_START,
                    StateKey::FRAME_CHANNEL, StateKey::FRAME_CHANNEL_END));
        m_selectImage.reset(
            new DataSelection(m_winId, StateKey::FRAME_IMAGE_START,
                    StateKey::FRAME_IMAGE, StateKey::FRAME_IMAGE_END));

        //See if there is any data that should be preloaded.
        QString dataCountStr = connector->getState( StateKey::DATA_COUNT, m_winId );
        bool validDataCount = false;
        int dataCount = dataCountStr.toInt( &validDataCount );
        if ( validDataCount && dataCount > 0 ){
            _loadData( dataCount );
        }

        //Initialize the commands
        _initializeCommands();

        //Load the view.
        _loadView( false );
    }

}

void DataController::setFrameChannel(const QString& val) {
    if (m_selectChannel != nullptr) {
        m_selectChannel->setIndex(val);
    }
}

void DataController::setFrameImage(const QString& val) {
    if (m_selectImage != nullptr) {
        m_selectImage->setIndex(val);
    }
}

QString DataController::getId() const {
    return m_winId;
}

QString DataController::getState(StateKey key) const {
    auto & globals = *Globals::instance();
    IConnector * connector = globals.connector();
    return connector->getState(key, m_winId);
}

void DataController::saveState() {
    //Note:: we need to save the number of data items that have been added
    //since otherwise, if data items have been deleted, their states will not
    //have been deleted, and we need to know when we read the states back in,
    //which ones represent valid data items and which ones do not.
    auto & globals = *Globals::instance();
    IConnector * connector = globals.connector();
    int dataCount = m_datas.size();
    QString dataCountStr = QString::number(dataCount);
    QString oldDataCount = connector->getState( StateKey::DATA_COUNT, m_winId );
    connector->setState(StateKey::DATA_COUNT, m_winId, dataCountStr);
    for (int i = 0; i < dataCount; i++) {
        m_datas[i]->saveState(m_winId, i);
    }
}



void DataController::_loadView( bool forceReload ) {

    //Determine the index of the data to load.
    int imageIndex = 0;
    if (m_selectImage != nullptr) {
        imageIndex = m_selectImage->getIndex();
    }

    if (imageIndex >= 0 && imageIndex < m_datas.size()) {
        if (m_datas[imageIndex] != nullptr) {

            //Determine the index of the channel to load.
            int frameIndex = 0;
            if (m_selectChannel != nullptr) {
                frameIndex = m_selectChannel->getIndex();
            }

            //Load the image.
            Nullable<QImage> res = m_datas[imageIndex]->load(frameIndex, forceReload);
            if (res.isNull()) {
                qDebug() << "Could not find any plugin to load image";
            } else {
                //Refresh the view.
                m_view->resetImage(res.val());
                auto & globals = *Globals::instance();
                IConnector * connector = globals.connector();
                connector->refreshView(m_view.get());
            }
        }
    } else {
        qDebug() << "Image index=" << imageIndex << " is out of range";
    }
}

void DataController::_initializeCommands() {
    auto & globals = *Globals::instance();
    IConnector * connector = globals.connector();

    //Listen for changes to channel selection.
    QString channelFrameKey = StateLibrary::instance()->getPath(
            StateKey::FRAME_CHANNEL, m_winId);
    connector->addStateCallback(channelFrameKey,
            [=] ( const QString & /*path*/, const QString & /*val*/) {
                _loadView(false);
            });

    //Listen for changes to data selection.
    QString imageFrameKey = StateLibrary::instance()->getPath(
            StateKey::FRAME_IMAGE, m_winId);
    connector->addStateCallback(imageFrameKey,
            [=] ( const QString & /*path*/, const QString & /*val*/) {
                _loadView(false);
            });

    //Listen for changes to the number of data that have been loaded.
    /*QString dataCountKey = StateLibrary::instance()->getPath(
            StateKey::DATA_COUNT, m_winId);
    connector->addStateCallback( dataCountKey,
            [=]( const QString& path, const QString& val ){
           bool validCount = false;
           int dataCount = val.toInt( &validCount );
           if ( validCount ){
               qDebug() << "_restoreState dataCount="<<dataCount<< " dataSize="<<m_datas.size();
               if ( m_datas.size() != dataCount ){
                   m_datas.clear();
               }
               _loadData( dataCount );
           }
    });*/

}

void DataController::_loadData( int dataCount ){
    auto & globals = *Globals::instance();
    IConnector * connector = globals.connector();
    for ( int i = 0; i < dataCount; i++ ){
        QString indexStr(m_winId + "-"+QString::number(i));
        QString fileName = connector->getState( StateKey::DATA_PATH, indexStr );
        addData( fileName );
    }
}

DataController::~DataController(){
    if ( m_winId.length() > 0 ){
        auto & globals = *Globals::instance();
        IConnector * connector = globals.connector();
        connector->unregisterView( m_winId );
    }
}
