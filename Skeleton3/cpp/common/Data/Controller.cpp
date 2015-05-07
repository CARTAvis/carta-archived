#include "State/ObjectManager.h"
#include "State/UtilState.h"
#include "Data/Controller.h"
#include "Data/DataLoader.h"
#include "Data/Colormap/Colormap.h"
#include "Data/DataSource.h"
#include "Data/Histogram.h"
#include "Data/Selection.h"
#include "Data/Region.h"
#include "Data/RegionRectangle.h"
#include "Data/Animator/Animator.h"
#include "Data/Util.h"
#include "ImageView.h"
#include "CartaLib/IImage.h"
#include "CartaLib/PixelPipeline/CustomizablePixelPipeline.h"

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <memory>
#include <set>

using namespace std;

namespace Carta {

namespace Data {

class Controller::Factory : public Carta::State::CartaObjectFactory {

public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Controller (path, id);
    }
};

const QString Controller::CLIP_VALUE_MIN = "clipValueMin";
const QString Controller::CLIP_VALUE_MAX = "clipValueMax";
const QString Controller::CLOSE_IMAGE = "closeImage";
const QString Controller::AUTO_CLIP = "autoClip";
const QString Controller::DATA = "data";
const QString Controller::DATA_COUNT = "dataCount";
const QString Controller::DATA_PATH = "dataPath";
const QString Controller::CURSOR = "formattedCursorCoordinates";
const QString Controller::CENTER = "center";
const QString Controller::POINTER_MOVE = "pointer-move";
const QString Controller::ZOOM = "zoom";
const QString Controller::REGIONS = "regions";
const QString Controller::PLUGIN_NAME = "CasaImageLoader";

const QString Controller::CLASS_NAME = "Controller";
bool Controller::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new Controller::Factory());

typedef Carta::State::UtilState UtilState;
typedef Carta::State::StateInterface StateInterface;

Controller::Controller( const QString& path, const QString& id ) :
        CartaObject( CLASS_NAME, path, id),
        m_selectChannel(nullptr),
        m_selectImage(nullptr),
        m_view(nullptr),
        m_stateData( UtilState::getLookup(path, StateInterface::STATE_DATA )),
        m_stateMouse(UtilState::getLookup(path, ImageView::VIEW)),
        m_viewSize( 400, 400){

    m_view.reset( new ImageView( path, QColor("pink"), QImage(), &m_stateMouse));
    m_reloadFrameQueued = false;
    m_repaintFrameQueued = false;

    _initializeSelections();

     connect( m_selectChannel, SIGNAL(indexChanged(bool)), this, SLOT(_scheduleFrameReload()));
     connect( m_selectImage, SIGNAL(indexChanged(bool)), this, SLOT(_scheduleFrameReload()));

     _initializeState();

     registerView(m_view.get());
     connect( m_view.get(), SIGNAL(resize(const QSize&)), this, SLOT(_viewResize(const QSize&)));

     //Load the view.
     _scheduleFrameReload();

     _initializeCallbacks();
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
        DataSource* targetSource = new DataSource();
        targetIndex = m_datas.size();
        connect( targetSource, SIGNAL(renderingDone(QImage)), this, SLOT(_renderingDone(QImage)));
        m_datas.append(targetSource);
        targetSource->viewResize( m_viewSize );

        //Update the data selectors upper bound based on the data.
        m_selectImage->setUpperBound(m_datas.size());
    }

    bool successfulLoad = m_datas[targetIndex]->setFileName(fileName );
    if ( successfulLoad ){
        int frameCount = m_datas[targetIndex]->getFrameCount();
        m_selectChannel->setUpperBound( frameCount );
        m_selectImage->setIndex(targetIndex);
        saveState();

        //Refresh the view of the data.
        _scheduleFrameReload();

        //Notify others there has been a change to the data.
        emit dataChanged( this );
    }
    else {
        _removeData( targetIndex );
    }
}

QString Controller::applyClips( double minIntensityPercentile, double maxIntensityPercentile ){
    QString result;
    bool clipsChanged = false;
    if ( minIntensityPercentile <= maxIntensityPercentile ){
        const double ERROR_MARGIN = 0.0001;
        if ( 0 <= minIntensityPercentile && minIntensityPercentile <= 1 ){
            double oldMin = m_state.getValue<double>(CLIP_VALUE_MIN );
            if ( qAbs(minIntensityPercentile - oldMin) > ERROR_MARGIN ){
                m_state.setValue<double>(CLIP_VALUE_MIN, minIntensityPercentile );
                clipsChanged = true;
            }
        }
        else {
            result = "Minimum intensity percentile invalid [0,1]: "+ QString::number( minIntensityPercentile);
        }
        if ( 0 <= maxIntensityPercentile && maxIntensityPercentile <= 1 ){
            double oldMax = m_state.getValue<double>(CLIP_VALUE_MAX);
            if ( qAbs(maxIntensityPercentile - oldMax) > ERROR_MARGIN ){
                m_state.setValue<double>(CLIP_VALUE_MAX, maxIntensityPercentile );
                clipsChanged = true;
            }
        }
        else {
            result = "Maximum intensity percentile invalid [0,1]: "+ QString::number( maxIntensityPercentile);
        }
        if( clipsChanged ){
            m_state.flushState();
            if ( m_view ){
                _scheduleFrameReload();
            }
        }
    }
    return result;
}


void Controller::clear(){
    unregisterView();
}

void Controller::_clearData(){
    for ( DataSource* source : m_datas ){
        delete source;
    }
    m_datas.clear();
}

QString Controller::closeImage( const QString& name ){
    int targetIndex = -1;
    QString result;
    int dataCount = m_datas.size();
    for ( int i = 0; i < dataCount; i++ ){
        if ( m_datas[i]->contains( name )){
            targetIndex = i;
            break;
        }
    }
    if ( targetIndex >= 0 ){
        this->_removeData( targetIndex );
        emit dataChanged( this );
    }
    else {
        result = "Could not find data to remove for name="+name;
    }
    return result;
}

int Controller::getChannelUpperBound() const {
    return m_selectChannel->getUpperBound();
}

int Controller::getFrameChannel() const {
    return m_selectChannel->getIndex();
}

bool Controller::getIntensity( int frameLow, int frameHigh, double percentile, double* intensity ) const{
    bool validIntensity = false;
    int imageIndex = m_selectImage->getIndex();
    if ( 0 <= imageIndex && imageIndex < m_datas.size()){
        validIntensity = m_datas[imageIndex]->getIntensity( frameLow, frameHigh, percentile, intensity );
    }
    return validIntensity;
}

double Controller::getPercentile( int frameLow, int frameHigh, double intensity ) const {
    double percentile = -1;
    int imageIndex = m_selectImage->getIndex();
    if ( 0 <= imageIndex && imageIndex < m_datas.size()){
        percentile = m_datas[imageIndex]->getPercentile( frameLow, frameHigh, intensity );
    }
    return percentile;
}


std::vector<std::shared_ptr<Image::ImageInterface>> Controller::getDataSources(){
    //For right now, we are only going to do a histogram of a single image.
    std::vector<std::shared_ptr<Image::ImageInterface>> images;
    int dataCount = m_datas.size();
    if ( dataCount > 0 ){
        int imageIndex = m_selectImage->getIndex();
        if ( 0 <= imageIndex && imageIndex < dataCount ){
            images.push_back( m_datas[imageIndex]->getImage());
        }
    }
    return images;
}

int Controller::getSelectImageIndex() const {
    int selectImageIndex = -1;
    if ( m_datas.size() >= 1 ){
        selectImageIndex = m_selectImage->getIndex();
    }
    return selectImageIndex;
}



QString Controller::getImageName(int index) const{
    QString name;
    if ( 0 <= index && index < m_datas.size()){
        DataSource* data = Controller::m_datas[index];
        name = data->getFileName();
    }
    return name;
}

std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> Controller::getPipeline() const {
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline(nullptr);
    int selectImageIndex = m_selectImage->getIndex();
    if ( 0 <= selectImageIndex && selectImageIndex < m_datas.size() ){
        pipeline = m_datas[selectImageIndex]->getPipeline();
    }
    return pipeline;
}

int Controller::getStackedImageCount() const {
    return m_selectImage->getUpperBound();
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


QString Controller::getStateString( const QString& sessionId, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        result = m_state.toString();
    }
    else if ( type == SNAPSHOT_DATA ){
        //Data state should include type and index for identification.
        //Data state should include the data of all DataSources (images loaded)
        //Data state should include DataSource that is selected
        //Data state should include channel that is selected.
        Carta::State::StateInterface dataState("");
        Carta::State::CartaObject* cartaObj = Util::findSingletonObject( DataLoader::CLASS_NAME );
        DataLoader* dataLoader = dynamic_cast<DataLoader*>(cartaObj);
        if ( dataLoader != nullptr ){
            QString rootDir = dataLoader->getRootDir( sessionId );
            Carta::State::StateInterface dataStateCopy( "");
            dataStateCopy.setState( m_stateData.toString());
            int dataCount = dataStateCopy.getArraySize( DATA );
            for ( int i = 0; i < dataCount; i++ ){
                QString lookup = Carta::State::UtilState::getLookup( DATA, i );
                QString fileName = dataStateCopy.getValue<QString>( lookup );
                dataStateCopy.setValue<QString>( lookup,
                        UtilState::getLookup(rootDir, fileName ));
            }
            dataState.setState( dataStateCopy.toString());
            dataState.setValue<QString>( StateInterface::OBJECT_TYPE, CLASS_NAME + StateInterface::STATE_DATA);
            dataState.setValue<int>(StateInterface::INDEX, getIndex());
            dataState.insertValue<QString>( Selection::CHANNEL, m_selectChannel->getStateString());
            dataState.insertValue<QString>( Selection::IMAGE, m_selectImage->getStateString());
            result = dataState.toString();
        }
    }
    return result;
}


QString Controller::getType(CartaObject::SnapshotType snapType) const {
    QString objType = CartaObject::getType( snapType );
    if ( snapType == SNAPSHOT_DATA ){
        objType = objType + Carta::State::StateInterface::STATE_DATA;
    }
    return objType;
}

void Controller::_initializeCallbacks(){
    //Listen for updates to the clip and reload the frame.

    addCommandCallback( "setClipValue", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {"clipValue"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        bool validClip = false;
        QString clipKey = *keys.begin();
        QString clipWithoutPercent = dataValues[clipKey].remove("%");
        double clipVal = dataValues[clipKey].toDouble(&validClip);
        if ( validClip ){
            double oldClipValMin = m_state.getValue<double>( CLIP_VALUE_MIN );
            double oldClipValMax = m_state.getValue<double>( CLIP_VALUE_MAX );
            double oldClipVal = oldClipValMax - oldClipValMin;
            const double ERROR_MARGIN = 0.000001;
            if ( qAbs( clipVal - oldClipVal) >= ERROR_MARGIN ){
                double leftOver = 1 - clipVal;
                double clipValMin = leftOver / 2;
                double clipValMax = clipVal + leftOver / 2;
                result = applyClips (clipValMin, clipValMax );
            }
        }
        else {
            result = "Invalid clip value: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setAutoClip", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {"autoClip"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
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

    QString pointerPath= UtilState::getLookup( getPath(), UtilState::getLookup( ImageView::VIEW, POINTER_MOVE));
    addStateCallback( pointerPath, [=] ( const QString& /*path*/, const QString& value ) {
        QStringList mouseList = value.split( " ");
        if ( mouseList.size() == 2 ){
            bool validX = false;
            int mouseX = mouseList[0].toInt( &validX );
            bool validY = false;
            int mouseY = mouseList[1].toInt( &validY );
            if ( validX && validY ){
                _updateCursor( mouseX, mouseY );
            }
        }

    });

    addCommandCallback( CLOSE_IMAGE, [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) ->QString {
        std::set<QString> keys = {"image"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString imageName = dataValues[*keys.begin()];
        QString result = closeImage( imageName );
                return result;
            });

    addCommandCallback( CENTER, [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) ->QString {
            auto vals = Util::string2VectorDouble( params );
            int count = vals.size();
            if ( count > 1 ) {
                updatePan( vals[0], vals[1]);
            }
            return "";
        });

    addCommandCallback( ZOOM, [=] (const QString & /*cmd*/,
            const QString & params, const QString & /*sessionId*/) ->QString {
        auto vals = Util::string2VectorDouble( params );
        if ( vals.size() > 2 ) {
            double centerX = vals[0];
            double centerY = vals[1];
            double z = vals[2];
            updateZoom( centerX, centerY, z );
        }
        return "";
    });

    addCommandCallback( "registerShape", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
        const QString TYPE( "type");
        const QString INDEX( "index");
        std::set<QString> keys = {TYPE, INDEX};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
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


void Controller::_initializeSelection( Selection* & selection ){
    CartaObject* selectObj = Util::createObject( Selection::CLASS_NAME );
    selection = dynamic_cast<Selection*>(selectObj);
}


void Controller::_initializeState(){

    //Set whether or not to auto clip
    //First the preference state.
    m_state.insertValue<bool>( AUTO_CLIP, true );
    m_state.insertValue<double>( CLIP_VALUE_MIN, 0.025 );
    m_state.insertValue<double>( CLIP_VALUE_MAX, 0.975 );
    m_state.flushState();

    //Now the data state.
    m_stateData.insertValue<int>(DATA_COUNT, 0 );
    m_stateData.insertArray(DATA, 0 );


    //For testing only.
    //_makeRegion( RegionRectangle::CLASS_NAME );
    int regionCount = m_regions.size();
    m_stateData.insertArray(REGIONS, regionCount );
    //_saveRegions();
    m_stateData.flushState();

    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(CURSOR, "");
    m_stateMouse.insertValue<QString>(POINTER_MOVE, "");
    m_stateMouse.insertValue<int>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<int>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.flushState();
}

void Controller::_loadView( ) {
    m_reloadFrameQueued = false;
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
            double clipValueMin = m_state.getValue<double>(CLIP_VALUE_MIN);
            double clipValueMax = m_state.getValue<double>(CLIP_VALUE_MAX);
            m_datas[imageIndex]->load(frameIndex, autoClip, clipValueMin, clipValueMax);
        }
        else {
            qDebug() << "Uninitialized image: "<<imageIndex;
        }
    }
    else {
        qDebug() << "Image index=" << imageIndex << " is out of range";
    }
}


QString Controller::_makeRegion( const QString& regionType ){
    QString shapePath = Region::makeRegion( regionType );
    if ( shapePath.size() > 0 ){
        Carta::State::ObjectManager* objManager = Carta::State::ObjectManager::objectManager();
        Carta::State::CartaObject* shapeObj = objManager->getObject( shapePath );
        shapePath = shapeObj->getPath();
        m_regions.append(dynamic_cast<Region*>(shapeObj));

     }
    return shapePath;
}

void Controller::_removeData( int index ){
    disconnect( m_datas[index]);
    int selectedImage = m_selectImage->getIndex();
    m_datas.removeAt( index );
    m_selectImage->setUpperBound( m_datas.size());
    if ( selectedImage == index ){
       m_selectImage->setIndex( 0 );
    }
    else if ( index < selectedImage ){
       m_selectImage->setIndex( selectedImage - 1);
    }
    this->_loadView();

    saveState();
}

void Controller::_render(){
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size()){
        m_datas[imageIndex]->render();
    }
}

void Controller::_renderingDone( QImage img ){
    _scheduleFrameRepaint( img );
}

void Controller::_repaintFrameNow(){
    m_view->scheduleRedraw();
    m_repaintFrameQueued = false;
}

void Controller::resetStateData( const QString& state ){
    //First we reset the data this controller is displaying
    qDebug() << "Controller resetStateData state="<<state;
    _clearData();
    Carta::State::StateInterface dataState( "");
    dataState.setState( state );
    int dataCount = dataState.getValue<int>(DATA_COUNT);
    for ( int i = 0; i < dataCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( DATA, i );
        QString fileName = dataState.getValue<QString>( lookup );
        this->addData( fileName );
    }
    //Now we need to restore the selected channel and image.
    QString channelState = dataState.getValue<QString>( Selection::CHANNEL );
    m_selectChannel->resetState( channelState );
    QString dataStateStr = dataState.getValue<QString>( Selection::IMAGE );
    m_selectImage ->resetState( dataStateStr );
    //Notify others there has been a change to the data.
    emit dataChanged( this );
}

void Controller::saveState() {
    bool stateChanged = false;
    int dataCount = m_datas.size();
    int oldDataCount = m_stateData.getValue<int>(DATA_COUNT );
    if ( oldDataCount != dataCount ){
        stateChanged = true;
        m_stateData.setValue<int>( DATA_COUNT, dataCount );
        //Insert the names of the data items for display purposes.
        m_stateData.resizeArray(DATA, dataCount, StateInterface::PreserveAll );
        for (int i = 0; i < dataCount; i++) {
            QString imageViewName = m_datas[i]->getImageViewName();
            QString dataKey = UtilState::getLookup( DATA, i);
            QString oldViewName;
            if ( i < oldDataCount ){
                oldViewName = m_stateData.getValue<QString>(dataKey);
            }
            if ( imageViewName != oldViewName ){
                m_stateData.setValue<QString>( dataKey, imageViewName );
            }
        }
    }
    /*int regionCount = m_regions.size();
    m_state.resizeArray( REGIONS, regionCount );
    _saveRegions();*/
    if ( stateChanged ){
        m_stateData.flushState();
    }
}

void Controller::_saveRegions(){
    int regionCount = m_regions.size();
    for ( int i = 0; i < regionCount; i++ ){
        QString arrayStr = UtilState::getLookup( REGIONS, i);
        QString regionType= m_regions[i]->getType();
        QString regionId = m_regions[i]->getPath();
        m_state.setObject( arrayStr );
        m_state.insertValue<QString>( UtilState::getLookup( arrayStr, "type"), regionType );
        m_state.insertValue<QString>( UtilState::getLookup( arrayStr, "id"), regionId );
    }
}

void Controller::_scheduleFrameRepaint( const QImage& img ){
    if ( m_datas.size() > 0 ){
        // if reload is already pending, do nothing
        if ( m_repaintFrameQueued ) {
            return;
        }
        m_view->resetImage( img );
        m_repaintFrameQueued = true;
        QMetaObject::invokeMethod( this, "_repaintFrameNow", Qt::QueuedConnection );
    }
}


void Controller::_scheduleFrameReload(){
    if ( m_datas.size() > 0  ){

        // if reload is already pending, do nothing
        if ( m_reloadFrameQueued ) {
            return;
        }
        m_reloadFrameQueued = true;
        QMetaObject::invokeMethod( this, "_loadView", Qt::QueuedConnection );
    }
}

void Controller::setColorInverted( bool inverted ){
    for ( DataSource* data : m_datas ){
        data->setColorInverted( inverted );
    }
    _render();
}

void Controller::setColorMap( const QString& name ){
    for ( DataSource* data : m_datas ){
        data->setColorMap( name );
    }
    _render();
}

void Controller::setColorReversed( bool reversed ){
    for ( DataSource* data : m_datas ){
        data->setColorReversed( reversed );
    }
    _render();
}

void Controller::setColorAmounts( double newRed, double newGreen, double newBlue ){
    for ( DataSource* data : m_datas ){
        data->setColorAmounts( newRed, newGreen, newBlue );
    }
    _render();
}

void Controller::setPixelCaching( bool enabled ){
    if ( m_datas.size() > 0 ){
        for ( DataSource* data : m_datas ){
            data->setPixelCaching( enabled );
        }
    }
}

void Controller::setCacheInterpolation( bool enabled ){
    if ( m_datas.size() > 0 ){
        for ( DataSource* data : m_datas ){
            data->setCacheInterpolation( enabled );
        }
    }
}

void Controller::setCacheSize( int size ){
    if ( m_datas.size() > 0 ){
        for ( DataSource* data : m_datas ){
            data->setCacheSize( size );
        }
    }
}


void Controller::setFrameChannel(int value) {
    if (m_selectChannel != nullptr) {
        int oldIndex = m_selectChannel->getIndex();
        if ( value != oldIndex ){
            m_selectChannel->setIndex(value);
            _updateCursorText( true );
            emit channelChanged( this );
        }
    }
}

void Controller::setFrameImage( int val) {
    if (m_selectImage != nullptr) {
        int oldIndex = m_selectImage->getIndex();
        if ( oldIndex != val ){
            m_selectImage->setIndex(val);
            int imageIndex = m_selectImage->getIndex();
            if ( 0 <= imageIndex && imageIndex < m_datas.size() ){
                int upperBound = m_datas[imageIndex]->getFrameCount();
                m_selectChannel->setUpperBound( upperBound );
            }
            _updateCursorText( true );
            emit dataChanged( this );
        }
    }
}

void Controller::setGamma( double gamma ){
    for ( DataSource* data : m_datas ){
        data->setGamma( gamma );
    }
    _render();
}



void Controller::setTransformData( const QString& name ){
    for ( DataSource* data : m_datas ){
        data->setTransformData( name );
    }
    _render();
}


void Controller::_updateCursor( int mouseX, int mouseY ){
    if ( m_datas.size() == 0 ){
        return;
    }
    int oldMouseX = m_stateMouse.getValue<int>( ImageView::MOUSE_X );
    int oldMouseY = m_stateMouse.getValue<int>( ImageView::MOUSE_Y );
    if ( oldMouseX != mouseX || oldMouseY != mouseY ){
        m_stateMouse.setValue<int>( ImageView::MOUSE_X, mouseX);
        m_stateMouse.setValue<int>( ImageView::MOUSE_Y, mouseY );
        _updateCursorText( false );
        m_stateMouse.flushState();
    }
}

void Controller::_updateCursorText(bool notifyClients ){
    QString formattedCursor;
    int imageIndex = m_selectImage->getIndex();
    int frameIndex = m_selectChannel->getIndex();
    int mouseX = m_stateMouse.getValue<int>(ImageView::MOUSE_X );
    int mouseY = m_stateMouse.getValue<int>(ImageView::MOUSE_Y );
    QString cursorText = m_datas[imageIndex]->getCursorText( mouseX, mouseY,frameIndex);
    if ( cursorText != m_stateMouse.getValue<QString>(CURSOR)){
        m_stateMouse.setValue<QString>( CURSOR, cursorText );
        if ( notifyClients ){
            m_stateMouse.flushState();
        }
    }
}

void Controller::updateZoom( double centerX, double centerY, double zoomFactor ){

    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 ){
        //Remember where the user clicked
        QPointF clickPtScreen( centerX, centerY);
        bool validImage = false;
        QPointF clickPtImageOld = m_datas[imageIndex]->getImagePt( clickPtScreen, &validImage );
        if ( validImage ){
            //Set the zoom
            double newZoom = 1;
            double oldZoom = m_datas[imageIndex]->getZoom();
            if ( zoomFactor < 0 ) {
                newZoom = oldZoom / 0.9;
            }
            else {
                newZoom = oldZoom * 0.9;
            }
            for (DataSource* data : m_datas ){
                data->setZoom( newZoom );
            }

            // what is the new image pixel under the mouse cursor?
            QPointF clickPtImageNew = m_datas[imageIndex]->getImagePt( clickPtScreen, &validImage );

            // calculate the difference
            QPointF delta = clickPtImageOld - clickPtImageNew;

            // add the delta to the current center
            QPointF currCenter = m_datas[imageIndex]->getCenter();
            QPointF newCenter = currCenter + delta;
            for ( DataSource* data : m_datas ){
                data->setPan( newCenter.x(), newCenter.y() );
            }
            _render();
        }
    }
}

void Controller::updatePan( double centerX , double centerY){
    int imageIndex = m_selectImage->getIndex();
    if ( imageIndex >= 0 && imageIndex < m_datas.size()){
        bool validImage = false;
        QPointF oldImageCenter = m_datas[imageIndex]-> getImagePt( { centerX, centerY }, &validImage );
        if ( validImage ){
            for ( DataSource* data : m_datas ){
                data->setPan( oldImageCenter.x(), oldImageCenter.y() );
            }
            _render();
            _updateCursorText( true );
        }
    }
}

void Controller::_viewResize( const QSize& newSize ){
    for ( int i = 0; i < m_datas.size(); i++ ){
        m_datas[i]->viewResize( newSize );
    }
    m_viewSize = newSize;
    _render();
}

Controller::~Controller(){
    clear();
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    if ( m_selectChannel != nullptr){
        objMan->destroyObject(m_selectChannel->getId());
        m_selectChannel = nullptr;
    }
    if ( m_selectImage != nullptr ){
        objMan->destroyObject( m_selectImage->getId());
        m_selectImage = nullptr;
    }
    _clearData();


    for ( Region* region : m_regions ){
        objMan->destroyObject( region->getId());
    }
    m_regions.clear();
}

}
}
