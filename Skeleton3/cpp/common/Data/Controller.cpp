#include "State/ObjectManager.h"

#include "Data/Controller.h"
#include "Data/Colormap.h"
#include "Data/DataSource.h"
#include "Data/Histogram.h"
#include "Data/Selection.h"
#include "Data/Region.h"
#include "Data/RegionRectangle.h"
#include "Data/Animator.h"
#include "Data/Util.h"
#include "ImageView.h"
#include "CartaLib/IImage.h"

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <memory>
#include <set>

using namespace std;

namespace Carta {

namespace Data {

class Controller::Factory : public CartaObjectFactory {

public:

    CartaObject * create (const QString & path, const QString & id)
    {
        return new Controller (path, id);
    }
};

const QString Controller::CLIP_VALUE = "clipValue";
const QString Controller::AUTO_CLIP = "autoClip";
const QString Controller::DATA_COUNT = "dataCount";
const QString Controller::DATA_PATH = "dataPath";
const QString Controller::CURSOR = "formattedCursorCoordinates";
const QString Controller::REGIONS = "regions";
const QString Controller::PLUGIN_NAME = "CasaImageLoader";

const QString Controller::CLASS_NAME = "Controller";
bool Controller::m_registered =
    ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Controller::Factory());

Controller::Controller( const QString& path, const QString& id ) :
        CartaObject( CLASS_NAME, path, id),
        m_selectChannel(nullptr),
        m_selectImage(nullptr),
        m_view(nullptr),
        m_stateMouse(path + StateInterface::DELIMITER+ImageView::VIEW){

    m_view.reset( new ImageView( path, QColor("pink"), QImage(), &m_stateMouse));
    _initializeSelections();

     connect( m_selectChannel.get(), SIGNAL(indexChanged(bool)), this, SLOT(_loadView(bool)));
     connect( m_selectImage.get(), SIGNAL(indexChanged(bool)), this, SLOT(_loadView(bool)));


     _initializeCallbacks();
     registerView(m_view.get());

     //Load the view.
     _loadView( false );

     _initializeState();
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
        CartaObject* dataSource = Util::createObject( DataSource::CLASS_NAME );
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

void Controller::colorMapChanged( const QString& name ){
    for ( std::shared_ptr<DataSource> data : m_datas ){
        data->setColorMap( name );
    }
    _loadView( true );
}

NdArray::RawViewInterface *  Controller::getRawData( const QString& fileName, int channel ) const {
    NdArray::RawViewInterface * rawData = nullptr;
    for ( std::shared_ptr<DataSource> data : m_datas ){
        if ( data->contains( fileName )){
            rawData = data->getRawData( channel );
            break;
        }
    }
    return rawData;
}

void Controller::setColorInverted( bool inverted ){
    for ( std::shared_ptr<DataSource> data : m_datas ){
        data->setColorInverted( inverted );
    }
    _loadView( true );
}

void Controller::setColorReversed( bool reversed ){
    for ( std::shared_ptr<DataSource> data : m_datas ){
        data->setColorReversed( reversed );
    }
    _loadView( true );
}

std::vector<std::shared_ptr<Image::ImageInterface>> Controller::getDataSources(){

    std::vector<std::shared_ptr<Image::ImageInterface>> images(m_datas.count());

    for( int i=0; i<m_datas.count(); ++i ){ 
        images[i] = m_datas[i].get()->getImage();
    }
    return images;
}

int Controller::getSelectImageIndex(){
    return m_selectImage->getIndex();
}

QString Controller::getImageName(int index){
    QString name = "";
    if(m_datas.size()>0){
        std::shared_ptr<DataSource> data = Controller::m_datas[index];
        name = data->getFileName();
    }
    return name;
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
        std::set<QString> keys = {"clipValue"};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        bool validClip = false;
        QString clipKey = *keys.begin();
        QString clipWithoutPercent = dataValues[clipKey].remove("%");
        double clipVal = dataValues[clipKey].toDouble(&validClip);
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
        return "";
    });

    addCommandCallback( "setAutoClip", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {"autoClip"};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString clipKey = *keys.begin();
        bool autoClip = false;
        if ( dataValues[clipKey] == "true"){
            autoClip = true;
        }
        bool oldAutoClip = m_state.getValue<bool>(AUTO_CLIP );
        if ( autoClip != oldAutoClip ){
            m_state.setValue<bool>( AUTO_CLIP, autoClip );
            m_state.flushState();
        }
        return "";
    });

    addCommandCallback( "updateCursor", [=] (const QString & /*cmd*/,
                            const QString & /*params*/, const QString & /*sessionId*/) -> QString {
        _updateCursor();
        return "";
    });






    addCommandCallback( "registerShape", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
        const QString TYPE( "type");
        const QString INDEX( "index");
        std::set<QString> keys = {TYPE, INDEX};
        std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
        QString shapePath;
        bool validIndex = false;
        int index = dataValues[INDEX].toInt( &validIndex );
        if ( validIndex ){
            int regionCount = m_regions.size();
            if ( 0 <= index && index < regionCount ){
                //Measure the index from the end.
                shapePath = m_regions[index]->getPath();
            }
            else {
                shapePath = _makeRegion( dataValues[TYPE]);
                if ( shapePath.size() == 0 ){
                    qDebug()<<"Error registerShape unsupported shape: "<<params;
                }
                else {
                    saveState();
                }
            }
        }
        return shapePath;
    });
}


void Controller::_initializeSelections(){
    _initializeSelection( m_selectChannel );
    _initializeSelection( m_selectImage );
}


void Controller::_initializeSelection( std::shared_ptr<Selection> & selection ){
    CartaObject* selectObj = Util::createObject( Selection::CLASS_NAME );
    selection.reset( dynamic_cast<Selection*>(selectObj) );
}


void Controller::_initializeState(){

    //Set whether or not to auto clip
    m_state.insertValue<bool>( AUTO_CLIP, true );
    m_state.insertValue<double>( CLIP_VALUE, 0.95 );
    m_state.insertValue<int>(DATA_COUNT, 0 );


    //For testing only.
    //_makeRegion( RegionRectangle::CLASS_NAME );
    int regionCount = m_regions.size();
    m_state.insertArray(REGIONS, regionCount );
    //_saveRegions();
    m_state.flushState();

    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(CURSOR, "");
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.flushState();

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
            QImage res = m_datas[imageIndex]->load(frameIndex, forceReload, autoClip, clipValue);
            if (res.isNull()) {
                qDebug() << "Could not find any plugin to load image";
            } else {
                //Refresh the view.
                m_view->resetImage(res);
                refreshView( m_view.get());
            }
        }
    } else {
        qDebug() << "Image index=" << imageIndex << " is out of range";
    }
}

QString Controller::_makeRegion( const QString& regionType ){
    QString shapePath = Region::makeRegion( regionType );
    if ( shapePath.size() > 0 ){
        ObjectManager* objManager = ObjectManager::objectManager();
        CartaObject* shapeObj = objManager->getObject( shapePath );
        shapePath = shapeObj->getPath();
        std::shared_ptr<Region> target( dynamic_cast<Region*>(shapeObj) );
        m_regions.append(target);

     }
    return shapePath;
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
    int regionCount = m_regions.size();
    m_state.resizeArray( REGIONS, regionCount );
    _saveRegions();
    m_state.flushState();
}

void Controller::_saveRegions(){
    int regionCount = m_regions.size();
    for ( int i = 0; i < regionCount; i++ ){
        QString arrayStr = REGIONS + StateInterface::DELIMITER + QString::number(i);
        QString regionType= m_regions[i]->getType();
        QString regionId = m_regions[i]->getPath();
        m_state.setObject( arrayStr );
        m_state.insertValue<QString>( arrayStr + StateInterface::DELIMITER + "type", regionType );
        m_state.insertValue<QString>( arrayStr + StateInterface::DELIMITER + "id", regionId );
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
    QString mouseYStr = m_stateMouse.getValue<QString>( ImageView::MOUSE_Y );
    int mouseY = mouseYStr.toInt( &validInt );
    if ( !validInt ){
        return;
    }
    int imageIndex = m_selectImage->getIndex();
    int frameIndex = m_selectChannel->getIndex();
    QStringList list = m_datas[imageIndex]->formatCoordinates( mouseX, mouseY, frameIndex );
    m_stateMouse.setValue<QString>( CURSOR, list.join("\n").toHtmlEscaped());

    m_stateMouse.flushState();
}

Controller::~Controller(){
    clear();
}

}
}
