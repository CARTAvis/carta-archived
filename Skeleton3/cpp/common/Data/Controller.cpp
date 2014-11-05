#include "State/ObjectManager.h"

#include "Data/Controller.h"
#include "Data/DataSource.h"
#include "Data/Selection.h"
#include "Data/Animator.h"
#include "Data/Util.h"
#include "ImageView.h"

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <memory>

using namespace std;

const QString Controller::CLIP_VALUE = "clipValue";
const QString Controller::AUTO_CLIP = "autoClip";
const QString Controller::DATA_COUNT = "dataCount";
const QString Controller::DATA_PATH = "dataPath";
const QString Controller::CURSOR = "formattedCursorCoordinates";

const QString Controller::CLASS_NAME = "Controller";
bool Controller::m_registered =
    ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Controller::Factory());

Controller::Controller( const QString& path, const QString& id ) :
        CartaObject( CLASS_NAME, path, id),
        m_selectChannel(nullptr),
        m_selectImage(nullptr),
        m_view(nullptr){
    m_view.reset( new ImageView( path, QColor("pink"), QImage(), &m_state));
    _initializeSelections();
    _initializeState();

     connect( m_selectChannel.get(), SIGNAL(indexChanged(bool)), this, SLOT(_loadView(bool)));
     connect( m_selectImage.get(), SIGNAL(indexChanged(bool)), this, SLOT(_loadView(bool)));

     _initializeCallbacks();

     registerView(m_view.get());

     //Load the view.
     _loadView( false );
}


void Controller::addData(const QString& fileName) {
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
        //std::shared_ptr<DataSource> targetSource(new DataSource(fileName));
        ObjectManager* objManager = ObjectManager::objectManager();
        QString dataSourceId = objManager->createObject( DataSource::CLASS_NAME );
        CartaObject* dataSource = objManager->getObject( dataSourceId );
        std::shared_ptr<DataSource> targetSource( dynamic_cast<DataSource*>(dataSource));
        targetIndex = m_datas.size();
        m_datas.push_back(targetSource);

        //Update the data selectors upper bound and index based on the data.
        m_selectImage->setUpperBound(m_datas.size());

        saveState();
    }

    bool successfulLoad = m_datas[targetIndex]->setFileName(fileName );
    if ( successfulLoad ){
        int frameCount = m_datas[targetIndex]->getFrameCount();
        m_selectChannel->setUpperBound( frameCount );
        m_selectImage->setIndex(targetIndex);

        //Refresh the view of the data.
        _loadView( false );

        //Notify others there has been a change to the data.
        emit dataChanged();
    }
    else {
        m_datas.removeAt( targetIndex );
    }
}

void Controller::clear(){
    unregisterView();
}


int Controller::getState( const QString& type, const QString& key ){

    int value = -1;
    if ( type == Selection::IMAGE ){
        value = m_selectImage->getState( key );
    }
    else if ( type == Selection::CHANNEL ){
        value = m_selectChannel->getState( key );
    }
    else {
        qDebug() << "DataController::getState unrecognized type="<<type;
    }
    return value;
}


QString Controller::getStateString() const{
    StateInterface writeState( m_state );
    writeState.insertObject( Selection::SELECTIONS );
    writeState.insertObject(Selection::SELECTIONS+StateInterface::DELIMITER + Selection::CHANNEL, m_selectChannel->getStateString());
    writeState.insertObject(Selection::SELECTIONS+StateInterface::DELIMITER + Selection::IMAGE, m_selectImage->getStateString());
    return writeState.toString();
}


void Controller::_initializeCallbacks(){
    //Listen for updates to the clip and reload the frame.

    addCommandCallback( "setClipValue", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        QList<QString> keys = {"clipValue"};
        QVector<QString> dataValues = Util::parseParamMap( params, keys );
        if ( dataValues.size() == keys.size()){
            bool validClip = false;
            QString clipWithoutPercent = dataValues[0].remove("%");
            double clipVal = dataValues[0].toDouble(&validClip);
            if ( validClip ){
                int oldClipVal = m_state.getValue<double>( CLIP_VALUE );
                if ( oldClipVal != clipVal ){
                    m_state.setValue<double>( CLIP_VALUE, clipVal );
                    m_state.flushState();
                    if ( m_view ){
                        _loadView( true );
                    }
                }
            }
            else {
                qDebug() << "Invalid clip value: "<<params;
            }
        }
        return "";
    });

    addCommandCallback( "setAutoClip", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            QList<QString> keys = {"autoClip"};
            QVector<QString> dataValues = Util::parseParamMap( params, keys );
            if ( dataValues.size() == keys.size()){
                bool autoClip = false;
                if ( dataValues[0] == "true"){
                    autoClip = true;
                }
                bool oldAutoClip = m_state.getValue<bool>(AUTO_CLIP );
                if ( autoClip != oldAutoClip ){
                    m_state.setValue<bool>( AUTO_CLIP, autoClip );
                    m_state.flushState();
                }
            }
            return "";
        });

    addCommandCallback( "updateCursor", [=] (const QString & /*cmd*/,
                            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        _updateCursor();
        return "";
    });
}


void Controller::_initializeSelections(){
    _initializeSelection( m_selectChannel );
    _initializeSelection( m_selectImage );
}


void Controller::_initializeSelection( std::shared_ptr<Selection> & selection ){
    ObjectManager* objManager = ObjectManager::objectManager();
    QString selectId = objManager->createObject( Selection::CLASS_NAME );
    CartaObject* selectObj = objManager->getObject( selectId );
    selection.reset( dynamic_cast<Selection*>(selectObj) );
}


void Controller::_initializeState(){
    //Set whether or not to auto clip
    m_state.insertValue<bool>( AUTO_CLIP, true );
    m_state.insertValue<double>( CLIP_VALUE, 0.95 );
    m_state.insertValue<int>(DATA_COUNT, 0 );
    m_state.insertValue<QString>(CURSOR, "");
    m_state.insertObject( ImageView::MOUSE );
    m_state.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_state.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_state.flushState();
}


void Controller::_loadView( bool forceReload ) {

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
            bool autoClip = m_state.getValue<bool>(AUTO_CLIP);
            double clipValue = m_state.getValue<double>(CLIP_VALUE);
            Nullable<QImage> res = m_datas[imageIndex]->load(frameIndex, forceReload, autoClip, clipValue);
            if (res.isNull()) {
                qDebug() << "Could not find any plugin to load image";
            } else {
                //Refresh the view.
                m_view->resetImage(res.val());
                refreshView( m_view.get());
            }
        }
    } else {
        qDebug() << "Image index=" << imageIndex << " is out of range";
    }
}


void Controller::saveState() {
    //Note:: we need to save the number of data items that have been added
    //since otherwise, if data items have been deleted, their states will not
    //have been deleted, and we need to know when we read the states back in,
    //which ones represent valid data items and which ones do not.

    int dataCount = m_datas.size();
    m_state.setValue<int>( DATA_COUNT, dataCount );
    for (int i = 0; i < dataCount; i++) {
        m_datas[i]->saveState(/*m_winId, i*/);
    }
}


void Controller::setFrameChannel(const QString& val) {
    if (m_selectChannel != nullptr) {
        m_selectChannel->setIndex(val);
    }
}

void Controller::setFrameImage(const QString& val) {
    if (m_selectImage != nullptr) {
        m_selectImage->setIndex(val);
    }
}


void Controller::_updateCursor(){
    QString formattedCursor;
    QString mouseXStr = m_state.getValue<QString>( ImageView::MOUSE_X );
    bool validInt = false;

    int mouseX = mouseXStr.toInt(&validInt );
    if ( !validInt ){
        return;
    }
    QString mouseYStr = m_state.getValue<QString>( ImageView::MOUSE_Y );
    int mouseY = mouseYStr.toInt( &validInt );
    if ( !validInt ){
        return;
    }
    int dataIndex = m_selectImage->getIndex();

    int imageDims = this->m_datas[dataIndex]->getDimensions();
    auto pixCoords = std::vector<double>( imageDims, 0.0);
    pixCoords[0] = mouseX;
    pixCoords[1] = mouseY;
    if( pixCoords.size() > 2) {
        int frameIndex = m_selectChannel->getIndex();
        pixCoords[2] = frameIndex;
    }
    auto list = m_coordinateFormatter->formatFromPixelCoordinate( pixCoords);
    m_state.setValue<QString>( CURSOR, list.join("\n").toHtmlEscaped());
    m_state.flushState();
}

Controller::~Controller(){
    clear();
}
